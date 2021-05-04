#ifndef THREAD_ARGUMENTS_H
#define THREAD_ARGUMENTS_H

#include <vector>
#include "CombineTask.h"
#include "../barrier.h"


class ParmaConcurrentMap;
struct idxXYZ;

class ThreadArguments
{
 public:

  ParmaConcurrentMap* maps;
  std::vector<idxXYZ>* dataSplits;

  int splitNumber;
  double phase_two_duration;
  
  double cost_localPCL;
  double cost_totalLocalCombine;
  double cost_duration;

  int numberOfSuccessful_CAS_D_mat;
  int numberOfUnsuccessful_CAS_D_mat;
  int numberOfMisses_D_mat;

  volatile int* localClusteringTasks;
  volatile int* localRelabelingTasks;
  //  volatile int* mapLocks;

  std::vector<CombineTask*>* combineTasks;
  
  
  //TODO  CombineAndLinkageStats* stats;

  barrier_t* barrier;

  bool useDelimitingBox;
};


#endif
