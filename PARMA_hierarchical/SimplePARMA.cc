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

  for (int i = 1; i < numberOfSplits; i++)
    {
      this->maps[0].combine(maps[i]);
      //TODO perform path shortcutting: maps[0].shortcut
      //TODO barrier before relabeling
    }

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
