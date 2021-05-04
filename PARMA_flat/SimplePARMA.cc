#include "SimplePARMA.h"

double SimplePARMA::executePARMA()
{
  double start_duration = omp_get_wtime();
  
  split_ptCloud();

  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      PARMA::performLocalClustering(&(dataSplits[splitNumber]),
				    &(maps[splitNumber]),
				    1000*(splitNumber+1),
				    useDelimitingBox
				    );
    }

  for (int i = 0; i < combineTasks->size(); i++)
    {
      maps[combineTasks->at(i)->index1].combine(maps[combineTasks->at(i)->index2]);
      combineTasks->at(i)->status = 3;
    }
  //TODO barrier before relabeling
  //exportClusteringLabels("before.idx");  
  for (int splitNumber = 0; splitNumber < numberOfSplits; splitNumber++)
    {
      PARMA::performLocalRelabeling(&(dataSplits[splitNumber]),
				    &(maps[splitNumber])
				    );
    }
  
  double stop_duration = omp_get_wtime();

  std::cout << "Simple PARMA took: " << stop_duration - start_duration << " seconds.\n";

  return (stop_duration - start_duration);
}
