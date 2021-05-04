#ifndef PARMA_FILE_H
#define PARMA_FILE_H

#include "CombineTask.h"
#include "ThreadArguments.h"
#include "../core_lib/idxXYZ.h"
#include "../globals/globals.h"
#include "../core_lib/ParmaConcurrentMap.h"

#include <omp.h>
#include <vector>

class PARMA
{
 public:

  pthread_t* pid;
  barrier_t* barrier;
  ThreadArguments *threadData;

  std::vector<idxXYZ>* ptCloud;
  std::vector<idxXYZ>* dataSplits;
  std::vector<CombineTask*>* combineTasks;
  
  bool useDelimitingBox;
  bool randomShuffle; /* TODO */
  
  ParmaConcurrentMap* maps;
  
  static void performLocalClustering(std::vector<idxXYZ>*, ParmaConcurrentMap*, int, bool var =  true);
  static void performLocalRelabeling(std::vector<idxXYZ>*, ParmaConcurrentMap*);
  
  PARMA();
  
  void load_ptCloud(char*);
  void split_ptCloud();
  void reclusterLocalNoisePoints();/* TODO */
  void exportClusteringLabels(char*);
  
  virtual double executePARMA() = 0;
  double getLongestPhaseTwoDuration();
};

#endif
