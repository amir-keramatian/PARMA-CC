#include "FlexiXHierarchicalPARMA.h"
#include <math.h>

double FlexiXHierarchicalPARMA::executePARMA()
{
  split_ptCloud();
  
  double start_duration = omp_get_wtime();
  
  volatile int* localClusteringTasks = new int[numberOfSplits];
  volatile int* localRelabelingTasks = new int[numberOfSplits];
  volatile int* combineTasks = new int[numberOfSplits];
  volatile int* totalNumberOfCombines = new int(0);
  
  //  tbb::concurrent_queue<int>* ZZZ = new tbb::concurrent_queue<int>();
  boost::lockfree::queue<int, boost::lockfree::capacity<600> >* ZZZ = new boost::lockfree::queue<int, boost::lockfree::capacity<600> >();

  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      localClusteringTasks[splitNumber] = 0;
      localRelabelingTasks[splitNumber] = 0;
      combineTasks[splitNumber] = 0;
    }

  for (int threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {      
      threadData[threadNumber].maps			= this->maps;
      threadData[threadNumber].dataSplits  		= this->dataSplits;
      threadData[threadNumber].localClusteringTasks     = localClusteringTasks;
      threadData[threadNumber].localRelabelingTasks     = localRelabelingTasks;
      threadData[threadNumber].combineTasks 		= combineTasks;
      threadData[threadNumber].useDelimitingBox		= this->useDelimitingBox;
      threadData[threadNumber].ZZZ			= ZZZ;
      threadData[threadNumber].totalNumberOfCombines	= totalNumberOfCombines;
      
      pthread_create(&pid[threadNumber],
		     NULL,
		     func_thread,
		     &threadData[threadNumber]);
    }

  for (int threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      pthread_join(pid[threadNumber], NULL);
    }

  double stop_duration = omp_get_wtime();

  std::cout << "Flexi-PARMA_H took: " << stop_duration - start_duration << " seconds.\n";

  return (stop_duration - start_duration);
}

void* FlexiXHierarchicalPARMA::func_thread(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;

  double start_phase_two_duration = omp_get_wtime();
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      if (input->localClusteringTasks[splitNumber] == 0)
	{
	  if (__sync_bool_compare_and_swap (&(input->localClusteringTasks[splitNumber]),
					    0,
					    1) == true)
	    {
	      PARMA::performLocalClustering(&(input->dataSplits[splitNumber]),
					    &(input->maps[splitNumber]),
					    1000*(splitNumber+1),
					    input->useDelimitingBox
					    );
	      __sync_fetch_and_add(&(input->combineTasks[splitNumber/2]),  1);

	      if (__sync_bool_compare_and_swap (&(input->combineTasks[splitNumber/2]),
						2,
						3) == true)
		{
		  input->ZZZ->push(floor(splitNumber/2));
		}
	    }
	}
    }
  double stop_phase_two_duration = omp_get_wtime();
  input->phase_two_duration = stop_phase_two_duration - start_phase_two_duration;
  
  FlexiXHierarchicalPARMA::performSmartHierarchicalCombineTasks(inputArg);

  FlexiXHierarchicalPARMA::performLocalRelabelingTasks(inputArg);

  return NULL;
}

void* FlexiXHierarchicalPARMA::performSmartHierarchicalCombineTasks(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;
  
  while(*(input->totalNumberOfCombines) < numberOfSplits - 1)
    {
      int child, parent, index;
      
      bool successful = input->ZZZ->pop(index);

      if(successful)
	{
	  parent = minIndexInLeftSubtree(index);
	  child  = minIndexInRighttSubtree(index);

	  input->maps[parent].combine(input->maps[child]);
	  input->combineTasks[index] = 4;
	  /*std::cout << "combining " << parent << " and " << child << std::endl;*/
	  
	  __sync_fetch_and_add(input->totalNumberOfCombines, 1);

	  int pp = floor(index/2)+numberOfSplits/2;
	  
	  if (pp < numberOfSplits - 1)
	    {
	      __sync_fetch_and_add(&(input->combineTasks[pp]),  1);

	      if(__sync_bool_compare_and_swap
		 (&(input->combineTasks[pp]), 2, 3) == true)
		{
		  input->ZZZ->push(pp);
		}
	    }
	}/*end if successful*/
    }
  
  return NULL;
}

void* FlexiXHierarchicalPARMA::performLocalRelabelingTasks(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      if (input->localRelabelingTasks[splitNumber] == 0)
	{
	  if (__sync_bool_compare_and_swap (&(input->localRelabelingTasks[splitNumber]),
					    0,
					    1) == true)
	    {
	      for (int i = 0; i < input->dataSplits[splitNumber].size(); i++)
		{
		  int idx = input->dataSplits[splitNumber][i].clusterIDX;
		  if (idx != -1)
		    {
		      input->dataSplits[splitNumber][i].clusterIDX = 
			input->maps[splitNumber].getEllipsoidByIDX(idx)->findRoot()->id;
		    }
		}
	    }
	}
    }
  return NULL;
}

int minIndexInLeftSubtree(int index)
{
  if (2*index < numberOfSplits)
    return (2*index);

  return minIndexInLeftSubtree( (2*index)%numberOfSplits);
}

int minIndexInRighttSubtree(int index)
{
  if (2*index+1 < numberOfSplits)
    return (2*index+1);
    
  return (minIndexInLeftSubtree((2*index)%numberOfSplits+1));
}
