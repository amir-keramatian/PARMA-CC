#ifndef PARMA_FILE_H
#define PARMA_FILE_H

#include "../../core_lib/idxXYZ.h"
#include "../../globals/globals.h"

#include <omp.h>
#include <vector>

class Baseline
{
 public:
  std::vector<idxXYZ>* ptCloud;

  bool randomShuffle; /* TODO */
  
  Baseline();
  
  void load_ptCloud(char*);
  double executeBaseline();
  void exportClusteringLabels(char*);  
};

#endif
