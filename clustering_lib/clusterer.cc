#include "clusterer.h"

std::vector<pcl::PointIndices>* Clusterer::euclidean_clustering(std::vector<idxXYZ>* data,
								double eps, int minPts)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);

  int numberOfPoints = 0;

  std::vector<idxXYZ>::iterator dataIterator;

  for (dataIterator = data->begin(); dataIterator != data->end(); ++dataIterator)
    {
      cloud->points.push_back(pcl::PointXYZ((*dataIterator).x, (*dataIterator).y, (*dataIterator).z));
      ++numberOfPoints;
    }
  cloud->width = numberOfPoints; 
  cloud->height = 1;
  cloud->is_dense = false; 
  
  // Creating the KdTree object for the search method of the extraction
  pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
  tree->setInputCloud (cloud);

  std::vector<pcl::PointIndices>* cluster_indices = new std::vector<pcl::PointIndices>();
  pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
  ec.setClusterTolerance (eps);
  ec.setMinClusterSize (minPts);
  ec.setMaxClusterSize (75000000);
  ec.setSearchMethod (tree);
  ec.setInputCloud (cloud);
  ec.extract (*cluster_indices);
  
  return cluster_indices;
}

std::vector<pcl::PointIndices>* Clusterer::dbscan_clustering(std::vector<idxXYZ>* inputData,
						  double eps, int minPts)
{
  int NOISE = -10;

  std::vector<pcl::PointIndices>* clustering = new std::vector<pcl::PointIndices>();
  
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
  int numberOfPoints = 0;

  std::vector<idxXYZ>::iterator inputDataIterator;

  for (inputDataIterator = inputData->begin(); inputDataIterator != inputData->end(); ++inputDataIterator)
    {
      cloud->points.push_back(pcl::PointXYZ((*inputDataIterator).x, (*inputDataIterator).y, (*inputDataIterator).z));
      ++numberOfPoints;    
    }
  cloud->width = numberOfPoints; 
  cloud->height = 1;
  cloud->is_dense = false; 

  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud (cloud);

  std::vector<bool> processed(cloud->size(), false);
  std::vector<bool> touched(cloud->size(), false);
  
  pcl::PointCloud<pcl::PointXYZ>::iterator pointCloudIterator;
  int j = 0;

  std::vector<int> neighboursToGetExplored;
  neighboursToGetExplored.reserve(numberOfPoints);
  
  for (pointCloudIterator = cloud->begin(); pointCloudIterator != cloud->end(); ++pointCloudIterator)
    {
      int locationIndex = pointCloudIterator - cloud->begin();
      if ( processed[locationIndex] == false)
	{
	  std::vector<float> pointRadiusSquaredDistance;
	  neighboursToGetExplored.clear();
	  kdtree.radiusSearch(*pointCloudIterator, eps, neighboursToGetExplored, pointRadiusSquaredDistance);
	  processed[locationIndex] = true; touched[locationIndex] = true;
	  if (neighboursToGetExplored.size() >= minPts)
	    { // core-point: the beginning of a cluster
	      ++j;
	      pcl::PointIndices::Ptr thisClusterIndices(new pcl::PointIndices());
	  
	      (*inputData)[locationIndex].clusterIDX = j; thisClusterIndices->indices.push_back(locationIndex);

	      while (!neighboursToGetExplored.empty())
		{
		  int index_of_neighbour = neighboursToGetExplored.back();
		  neighboursToGetExplored.pop_back();

		  if (processed[index_of_neighbour] == true)
		    {
		      assert(	    (*inputData)[index_of_neighbour].clusterIDX != -1); //make sure it's not uninitialized
		      /*point at index_of_neighbour is either noise or edge.
			In either case no need to explore its neighbours. 
			If it's edge, it is already been clustered (in another segement) because the processed flag is set to true*/
		      /* if previously it was clustered as noise, now its label changes: */

		      if ((*inputData)[index_of_neighbour].clusterIDX == NOISE)
			{
			  (*inputData)[index_of_neighbour].clusterIDX = j;
			  thisClusterIndices->indices.push_back(index_of_neighbour);
			}
		      continue;
		    }
		  processed[index_of_neighbour] = true;
		  touched[index_of_neighbour] = true;
		  std::vector<int> pointIdxRadiusSearch;
		  kdtree.radiusSearch(*(cloud->begin() + index_of_neighbour), eps, pointIdxRadiusSearch, pointRadiusSquaredDistance);

		  if (pointIdxRadiusSearch.size() >= minPts) // this neighbour is a core point by itself. Therefore, its neighbours have to be explored
		    {
		      while(!pointIdxRadiusSearch.empty())
			{
			  int kirdex_of_neighbour = pointIdxRadiusSearch.back();
			  pointIdxRadiusSearch.pop_back();

			  if (touched[kirdex_of_neighbour] == false)
			    {
			      neighboursToGetExplored.push_back(kirdex_of_neighbour);
			      touched[kirdex_of_neighbour] = true;
			    }
			  if ((*inputData)[kirdex_of_neighbour].clusterIDX == NOISE){
			    (*inputData)[kirdex_of_neighbour].clusterIDX = j;
			    thisClusterIndices->indices.push_back(kirdex_of_neighbour);
			  }
			}
		    }
		  (*inputData)[index_of_neighbour].clusterIDX = j;
		  thisClusterIndices->indices.push_back(index_of_neighbour);
		}
	      clustering->push_back(*thisClusterIndices);
	      //#	    std::cout << "thisClusterIndices->indices.size() = " << thisClusterIndices->indices.size() << std::endl;

	    }// end core-point:...
	  else //if it's a non-core point
	    {
	      (*inputData)[locationIndex].clusterIDX = NOISE; //temporarily indicate the point as noise. it might change if this point is discovered later
	    }
	} //end if processed[locationIndex] == false
    }//end for all point in cloud

  for (int index = 0; index < inputData->size(); index++)
    {
      if ( (*inputData)[index].clusterIDX == NOISE)
	(*inputData)[index].clusterIDX = -1; //change noise indices to -1 (expected format for local relabeling)
    }

  return clustering;
}

std::vector<pcl::PointIndices>* Clusterer::extract(int algorithm, std::vector<idxXYZ>* data,
						   double eps, int minPts)
{
  if (algorithm == 1)
    return Clusterer::euclidean_clustering(data, eps, minPts);
  else if (algorithm == 2)
    return Clusterer::dbscan_clustering(data, eps, minPts);
}
