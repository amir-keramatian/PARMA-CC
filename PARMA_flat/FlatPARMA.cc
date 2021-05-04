#include "FlatPARMA.h"

double FlatPARMA::executePARMA()
{
  assert(numberOfThreads == numberOfSplits);

  split_ptCloud();
  
  double start_duration = omp_get_wtime();

  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {      
      threadData[splitNumber].maps		= this->maps;
      threadData[splitNumber].dataSplits       	= this->dataSplits;
      threadData[splitNumber].splitNumber      	= splitNumber;
      threadData[splitNumber].barrier		= barrier;
      threadData[splitNumber].useDelimitingBox	= this->useDelimitingBox;
      threadData[splitNumber].combineTasks	= this->combineTasks;
      
      pthread_create(&pid[splitNumber], NULL, func_thread, &threadData[splitNumber]);
    }
  
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      pthread_join(pid[splitNumber], NULL);
    }

  double stop_duration = omp_get_wtime();
  
  std::cout << "Flat PARMA took: " << stop_duration - start_duration << " seconds." << std::endl;

  return (stop_duration - start_duration);
}

void* FlatPARMA::func_thread(void* inputArg)
{
  ThreadArguments* input = (ThreadArguments*)inputArg;

  double tik = omp_get_wtime();
  PARMA::performLocalClustering(&(input->dataSplits[input->splitNumber]),
				&(input->maps[input->splitNumber]),
				1000*(input->splitNumber+1),
				input->useDelimitingBox
				);
  
  for (int i = 0; i < (input->combineTasks)->size(); i++)
    {
      if (input->combineTasks->at(i)->hasIndex(input->splitNumber) == true)
	{
	  __sync_fetch_and_add(&(input->combineTasks->at(i)->status),
			       +1);
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

  PARMA::performLocalRelabeling(&(input->dataSplits[input->splitNumber]),
  				&(input->maps[input->splitNumber])
  				);
  return NULL;
}

