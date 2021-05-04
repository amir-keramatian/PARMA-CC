#ifndef THREAD_ARGUMENTS_H
#define THREAD_ARGUMENTS_H
#include <vector>
#include "../barrier.h"
#include "tbb/concurrent_queue.h"
#include <boost/lockfree/queue.hpp>

class ParmaMap;
struct idxXYZ;

class ThreadArguments
{
 public:
  ParmaMap* maps;
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
  volatile int* mapLocks;
  volatile int* combineTasks;
  volatile int* totalNumberOfCombines;
  
  //TODO  CombineAndLinkageStats* stats;

  barrier_t* barrier;

  bool useDelimitingBox;

  //tbb::concurrent_queue<int>* ZZZ;
  boost::lockfree::queue<int, boost::lockfree::capacity<600> >* ZZZ;
  int* ZZZ_size;
};


#endif
