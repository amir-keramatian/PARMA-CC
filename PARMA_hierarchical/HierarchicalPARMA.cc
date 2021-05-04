#include "HierarchicalPARMA.h"

double HierarchicalPARMA::executePARMA()
{
  assert(numberOfThreads == numberOfSplits);

  split_ptCloud();
  
  barrier_t* barrier = new barrier_t();
  barrier_init(barrier, numberOfThreads);

  double start_duration = omp_get_wtime();
  
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {      
      threadData[splitNumber].maps		= this->maps;
      threadData[splitNumber].dataSplits       	= this->dataSplits;
      threadData[splitNumber].splitNumber      	= splitNumber;
      threadData[splitNumber].barrier		= barrier;
      threadData[splitNumber].useDelimitingBox	= this->useDelimitingBox;
      
      pthread_create(&pid[splitNumber], NULL, func_thread, &threadData[splitNumber]);
    }
  
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      pthread_join(pid[splitNumber], NULL);
    }

  double stop_duration = omp_get_wtime();
  
  std::cout << "PARMA_H took: " << stop_duration - start_duration << " seconds." << std::endl;

  return (stop_duration - start_duration);
}


void* HierarchicalPARMA::func_thread(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;

  double start_phase_two_duration = omp_get_wtime();
  PARMA::performLocalClustering(&(input->dataSplits[input->splitNumber]),
				&(input->maps[input->splitNumber]),
				1000*(input->splitNumber+1),
				input->useDelimitingBox
				);
  double stop_phase_two_duration = omp_get_wtime();
  input->phase_two_duration = stop_phase_two_duration - start_phase_two_duration;
  
  int whereAreMyChildren[numberOfThreads];

  for (int i = 0; i < numberOfThreads; i++)
    {
      whereAreMyChildren[i] = 0;
    }
  
  if (2*input->splitNumber + 1 < numberOfThreads)
    {
      whereAreMyChildren[2*input->splitNumber + 1] = 1;
    }
  if (2*input->splitNumber + 2 < numberOfThreads)
    {
      whereAreMyChildren[2*input->splitNumber + 2] = 1;
    }

  bool unprocessedChildExists;
  do{
    unprocessedChildExists = false;
    for(int index = 0; index < numberOfThreads; index++)
      {
	if (whereAreMyChildren[index] == 1)
	  { 
	    if(input->maps[index].ready == false)
	      {
		unprocessedChildExists = true;
	      }
	    else
	      {
		input->maps[input->splitNumber].combine(input->maps[index]);

		whereAreMyChildren[index] = 0; //remove index from the unprocessed list
	      }	
	  }
      }
  } while(unprocessedChildExists);

  input->maps[input->splitNumber].ready = true;

  barrier_cross(input->barrier);

  PARMA::performLocalRelabeling(&(input->dataSplits[input->splitNumber]),
				&(input->maps[input->splitNumber])
				);
  return NULL;
}

