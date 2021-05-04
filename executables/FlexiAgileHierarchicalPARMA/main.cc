#include <iostream>
#include <libgen.h>
#include "../../PARMA_hierarchical/FlexiAgileHierarchicalPARMA.h"
#include "../../globals/globals.h"

int main(int argc, char* argv[])
{
  parseTheArguments_PARMACC(argc, argv);
  
  char* clusteringResult = new char[500];
  sprintf(clusteringResult, "%s/labeling/%s_FlexiAgileHierarchicalPARMA_K_%d_S_%d.idx",
	  outputDir, basename(inputFile), numberOfThreads, numberOfSplits);
  
  char* clusteringStats = new char[500];
  sprintf(clusteringStats, "%s/stats/profile.txt",
	  outputDir);

  FlexiAgileHierarchicalPARMA *parma = new FlexiAgileHierarchicalPARMA();
  parma->load_ptCloud(inputFile);
  parma->useDelimitingBox = true;
  double duration = parma->executePARMA();
  double longestPhaseTwoDuration = parma->getLongestPhaseTwoDuration();
  parma->exportClusteringLabels(clusteringResult);
    
  std::ofstream profileStream;

  profileStream.open(clusteringStats, std::ofstream::out | std::ofstream::app);
  profileStream << duration
    		<< "\t\t"
		<< longestPhaseTwoDuration
		<< std::endl;
  profileStream.close();

  return 0;
}

