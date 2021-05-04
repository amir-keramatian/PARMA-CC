#ifndef COMBINE_AND_LINKAGE_STATS_H
#define COMBINE_AND_LINKAGE_STATS_H

class CombineAndLinkageStats{
 public:
  int numberOfEllipsoidComparisons;
  int numberOfEllipsoidLinkages;
  int numberOfEllipsoidLinkageFailures;
  double cost_pureCombine;
  
  CombineAndLinkageStats(){
    numberOfEllipsoidComparisons = 0;
    numberOfEllipsoidLinkages = 0;
    numberOfEllipsoidLinkageFailures = 0;
    cost_pureCombine = 0.0;
  }
};


#endif
