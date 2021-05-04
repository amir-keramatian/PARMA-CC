#include <iostream>
#include <libgen.h>
#include <cstdio>
#include <fstream>
#include "baseline.h"

int main(int argc, char* argv[])
{
  parseTheArguments_PARMACC(argc, argv);

  char* clusteringResult = new char[500];
  sprintf(clusteringResult, "%s/labeling/%s_baseline.idx",
	  outputDir, basename(inputFile));
  
  char* clusteringStats = new char[500];
  sprintf(clusteringStats, "%s/stats/profile_baseline.txt",
	  outputDir);
  
  Baseline *baseline = new Baseline();
  baseline->load_ptCloud(inputFile);
  double duration = baseline->executeBaseline();
  baseline->exportClusteringLabels(clusteringResult);
  
  std::ofstream profileStream;
  profileStream.open(clusteringStats, std::ofstream::out | std::ofstream::app);
  profileStream << duration
		<< std::endl;
  profileStream.close();

  return 0;
}

