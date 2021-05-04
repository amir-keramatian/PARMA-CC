#include <cassert>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <string.h>

#include "baseline.h"
#include "../../clustering_lib/clusterer.h"

Baseline::Baseline()
{
  ptCloud = NULL;
}

void Baseline::load_ptCloud(char* file)
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

void Baseline::exportClusteringLabels(char* file)
{
  std::ofstream outStream;
  outStream.open(file);

  for (int j = 0; j < ptCloud->size(); j++)
    {
      outStream << (*ptCloud)[j].clusterIDX << std::endl;
    }      

  outStream.close();
}

double Baseline::executeBaseline()
{
  std::vector<pcl::PointIndices>* cluster_indices;

  double start_duration = omp_get_wtime();
  cluster_indices = Clusterer::extract(localClusteringAlg, ptCloud, EPSILON, MinPts);
  double stop_duration = omp_get_wtime();
  
  std::cout << "Baseline took: " << stop_duration - start_duration << " seconds.\n";
  
  int j = 1;
  for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices->begin ();
       it != cluster_indices->end ();
       ++it)
    {
      for (std::vector<int>::const_iterator pit = it->indices.begin();
	   pit != it->indices.end();
	   ++pit)
	{
	  (*ptCloud)[*pit].clusterIDX = j;
	}
      j++;
    }

  return (stop_duration - start_duration);
}
