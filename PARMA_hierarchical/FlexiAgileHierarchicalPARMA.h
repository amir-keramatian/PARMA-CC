#ifndef FLEXI_MP_FILE_H
#define FLEXI_MP_FILE_H

#include "PARMA.h"

class FlexiAgileHierarchicalPARMA : public PARMA
{
 public:
  virtual double executePARMA();
  
  static void* func_thread(void*);
  static void* performSmartHierarchicalCombineTasks(void*);
  static void* performLocalRelabelingTasks(void*);
};

#endif
