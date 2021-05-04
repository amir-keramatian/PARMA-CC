#ifndef PARMA_FILE_H
#define PARMA_FILE_H

#include "ThreadArguments.h"
#include "../core_lib/idxXYZ.h"
#include "../globals/globals.h"
#include "../core_lib/ParmaMap.h"

#include <omp.h>
#include <vector>

class PARMA
{
 public:

  pthread_t *pid;
  ThreadArguments *threadData;
    
  std::vector<idxXYZ>* ptCloud;
  std::vector<idxXYZ>* dataSplits;

  double phase_two_duration;
  
  bool useDelimitingBox;
  bool randomShuffle; /* TODO */
  
  ParmaMap* maps;
  
  static void performLocalClustering(std::vector<idxXYZ>*, ParmaMap*, int, bool var =  true);
  static void performLocalRelabeling(std::vector<idxXYZ>*, ParmaMap*);
  
  PARMA();
  
  void load_ptCloud(char*);
  void split_ptCloud();
  void reclusterLocalNoisePoints();/* TODO */
  void exportClusteringLabels(char*);

  virtual double executePARMA() = 0;
  double getLongestPhaseTwoDuration();
};

#endif
