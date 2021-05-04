#include "FlexiAgileHierarchicalPARMA.h"

double FlexiAgileHierarchicalPARMA::executePARMA()
{
  split_ptCloud();
  
  double start_duration = omp_get_wtime();

  volatile int* localClusteringTasks = new int[numberOfSplits];
  volatile int* localRelabelingTasks = new int[numberOfSplits];

  volatile int* totalNumberOfCombines = new int(0);
  int* ZZZ_size = new int(0);

  boost::lockfree::queue<int, boost::lockfree::capacity<600> >* ZZZ = new boost::lockfree::queue<int, boost::lockfree::capacity<600> >();

  assert(numberOfSplits <= 600);
  
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      localClusteringTasks[splitNumber] = 0;
      localRelabelingTasks[splitNumber] = 0;
    }

  for (int threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {      
      threadData[threadNumber].maps			= this->maps;
      threadData[threadNumber].dataSplits  		= this->dataSplits;
      threadData[threadNumber].localClusteringTasks     = localClusteringTasks;
      threadData[threadNumber].localRelabelingTasks     = localRelabelingTasks;
      threadData[threadNumber].useDelimitingBox		= this->useDelimitingBox;
      threadData[threadNumber].ZZZ			= ZZZ;
      threadData[threadNumber].totalNumberOfCombines	= totalNumberOfCombines;
      /*threadData[threadNumber].ZZZ_size			= ZZZ_size;*/
      
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

  std::cout << "Flexi Agile Hierarchical took: " << stop_duration - start_duration << " seconds.\n";

  int last_component;  ZZZ->pop(last_component);
  bool flag = ZZZ->pop(last_component);
  assert(flag == false);

  return (stop_duration - start_duration);
}

void* FlexiAgileHierarchicalPARMA::func_thread(void* inputArg)
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
	      input->ZZZ->push(splitNumber);
	      /*__sync_fetch_and_add(input->ZZZ_size, 1);*/
	    }
	}
    }
  double stop_phase_two_duration = omp_get_wtime();
  input->phase_two_duration = stop_phase_two_duration - start_phase_two_duration;

  FlexiAgileHierarchicalPARMA::performSmartHierarchicalCombineTasks(inputArg);
  
  FlexiAgileHierarchicalPARMA::performLocalRelabelingTasks(inputArg);

  return NULL;
}

void* FlexiAgileHierarchicalPARMA::performSmartHierarchicalCombineTasks(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;
  
  while(*(input->totalNumberOfCombines) < numberOfSplits - 1)
    {
      /*if (*(input->ZZZ_size) >= 2)*/      
      int s1, s2;
      bool successful_s1 = input->ZZZ->pop(s1);

      if(successful_s1)	    
	{
	  bool successful_s2 = input->ZZZ->pop(s2);

	  if(successful_s2)
	    {
	      input->maps[s1].combine(input->maps[s2]);

	      __sync_fetch_and_add(input->totalNumberOfCombines, 1);

	    }
	  input->ZZZ->push(s1);
	}
    }

  return NULL;
}

void* FlexiAgileHierarchicalPARMA::performLocalRelabelingTasks(void* inputArg)
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
