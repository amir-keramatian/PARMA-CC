#include "FlexiFlatPARMA.h"

double FlexiFlatPARMA::executePARMA()
{
  split_ptCloud();
  
  double start_duration = omp_get_wtime();

  volatile int* localClusteringTasks = new int[numberOfSplits];
  volatile int* localRelabelingTasks = new int[numberOfSplits];

  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      localClusteringTasks[splitNumber] = 0;
      localRelabelingTasks[splitNumber] = 0;
    }

  
  for (int threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {      
      threadData[threadNumber].maps		= this->maps;
      threadData[threadNumber].dataSplits      	= this->dataSplits;
      threadData[threadNumber].barrier		= barrier;
      threadData[threadNumber].useDelimitingBox	= this->useDelimitingBox;
      threadData[threadNumber].combineTasks	= this->combineTasks;
      threadData[threadNumber].localClusteringTasks     = localClusteringTasks;
      threadData[threadNumber].localRelabelingTasks     = localRelabelingTasks;
      
      pthread_create(&pid[threadNumber], NULL, func_thread, &threadData[threadNumber]);
    }
  
  for (int threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      pthread_join(pid[threadNumber], NULL);
    }

  double stop_duration = omp_get_wtime();
  
  std::cout << "Flexi Flat PARMA took: " << stop_duration - start_duration << " seconds." << std::endl;

  return (stop_duration - start_duration);
}

void* FlexiFlatPARMA::func_thread(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;

  double tik = omp_get_wtime();
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
	      for (int i = 0; i < (input->combineTasks)->size(); i++)
		{
		  if (input->combineTasks->at(i)->hasIndex(splitNumber) == true)
		    {
		      __sync_fetch_and_add(&(input->combineTasks->at(i)->status),
					   +1);
		    }
		}
	    }
	}
    }
  double tok = omp_get_wtime();
  input->phase_two_duration = tok - tik;
  
  bool combineTaskLeftToDo;
  do
    {
      combineTaskLeftToDo = false;
      
      for (int i = 0; i < input->combineTasks->size(); i++)
	{
	  if (input->combineTasks->at(i)->status != 3)
	    {
	      combineTaskLeftToDo = true;
	    }

	  if (((input->combineTasks)->at(i))->status == 2)
	    {
	      if(__sync_bool_compare_and_swap( &( (input->combineTasks)->at(i)->status),
					       2,
					       3) == true)
		{
		  input->maps[input->combineTasks->at(i)->index1].combine
		    (input->maps[input->combineTasks->at(i)->index2]);
		}
	    }
	}  
    }while(combineTaskLeftToDo == true);

  barrier_cross(input->barrier);

  for (int i = 0; i < (input->combineTasks)->size(); i++)
    {
      assert(((input->combineTasks)->at(i))->status == 3);//debug assert
    }

  FlexiFlatPARMA::performLocalRelabelingTasks(inputArg);
  
  return NULL;
}

void* FlexiFlatPARMA::performLocalRelabelingTasks(void* inputArg)
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
