#include <cassert>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <string.h>

#include "PARMA.h"
#include "../clustering_lib/clusterer.h"

PARMA::PARMA()
{
  pid = new pthread_t[numberOfThreads];
  threadData = new ThreadArguments[numberOfThreads];
  ptCloud = NULL;
  dataSplits = NULL;
  maps = new ParmaMap[numberOfSplits];
  useDelimitingBox = true;
  phase_two_duration = 0;
}

void PARMA::load_ptCloud(char* file)
{
  assert(strlen(file) != 0);

  ptCloud = new std::vector<idxXYZ>();
  
  double x,y,z;

  std::ifstream ptCloudInFile(file);

  while ( ptCloudInFile >> x >> y >> z)
    {
      idxXYZ *tuple = new idxXYZ(x, y, z);
      this->ptCloud->push_back(*tuple);
    }

  assert(ptCloud->size() > 0);
}

void PARMA::split_ptCloud()
{
  assert(this->ptCloud != NULL);
  assert(this->ptCloud->size() > 0);
  
  dataSplits = new std::vector<idxXYZ>[numberOfSplits];

  int bucket = 0;
  int size_of_bucket = 0;
  int number_of_points_in_each_bucket = (ptCloud->size()/numberOfSplits)+1;
  
  for (int i = 0; i < ptCloud->size(); i++)
    {
      if (size_of_bucket >= number_of_points_in_each_bucket)
	{
	  bucket++;
	  size_of_bucket = 0;
	}
      dataSplits[bucket].push_back(ptCloud->at(i));
      size_of_bucket++;
    }

  int sum_partion_sizes = 0;
  int globalIndex = 0;

  for (int i = 0; i < numberOfSplits; i++)
    {
      assert(dataSplits[i].size() > 0);
      sum_partion_sizes+= dataSplits[i].size();

      for (int j = 0; j < dataSplits[i].size(); j++)
	{
	  assert(dataSplits[i][j].x == (*ptCloud)[globalIndex].x);
	  assert(dataSplits[i][j].y == (*ptCloud)[globalIndex].y);
	  assert(dataSplits[i][j].z == (*ptCloud)[globalIndex].z);
	  globalIndex++;
	}
    }
  assert(sum_partion_sizes == ptCloud->size());
}

void PARMA::performLocalRelabeling(std::vector<idxXYZ>* data, ParmaMap* map)
{
  assert(data != NULL);
  assert(map  != NULL);
  
  for (int i = 0; i < data->size(); i++)
    {
      if ((*data)[i].clusterIDX != -1)
	{
	  (*data)[i].clusterIDX = 
	    map->getEllipsoidByIDX((*data)[i].clusterIDX)->findRoot()->id;
	}
    }
}

void PARMA::exportClusteringLabels(char* file)
{
  std::ofstream outStream;
  outStream.open(file);
  
  for (int i = 0; i < numberOfSplits; i++)
    {
      assert(dataSplits[i].size() > 0);

      for (int j = 0; j < dataSplits[i].size(); j++)
	{
	  outStream << dataSplits[i][j].clusterIDX << std::endl;
	}      
    }

  outStream.close();
}

void PARMA::performLocalClustering(std::vector<idxXYZ>* data, ParmaMap* map,
				   int offset,
				   bool useDelimitingBox)
{
  assert(data != NULL);

  std::vector<pcl::PointIndices>* cluster_indices;
  cluster_indices = Clusterer::extract(localClusteringAlg, data, EPSILON, MinPts);

  int j = 1;
  
  for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices->begin ();
       it != cluster_indices->end ();
       ++it)
    {
      ParmaEllipsoid* ellipsoid = new ParmaEllipsoid(offset + j);
      ellipsoid->source = offset;
      
      for (std::vector<int>::const_iterator pit = it->indices.begin();
	   pit != it->indices.end();
	   ++pit)
	{      
	  (*data)[*pit].clusterIDX = (offset + j);
	  (*data)[*pit].clusterIDX_beforeCombine = (offset + j);
	  ellipsoid->addTuple((*data)[*pit]);
	}

      ellipsoid->finalize();

      if (useDelimitingBox == false)
	{
	  ellipsoid->db.deactivate();
	}
      map->insertEllipsoid(ellipsoid);

      j++;
    } 
}

double PARMA::getLongestPhaseTwoDuration()
{
  double longestPhaseTwoDuration = threadData[0].phase_two_duration;

  for (int threadNumber = 1; threadNumber < numberOfThreads; threadNumber++)
    {
      if (threadData[threadNumber].phase_two_duration > longestPhaseTwoDuration)
	longestPhaseTwoDuration = threadData[threadNumber].phase_two_duration;
    }
  
  return longestPhaseTwoDuration;
}
