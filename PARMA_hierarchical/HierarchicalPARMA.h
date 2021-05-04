#ifndef Hierarchical_PARMA_FILE_H
#define Hierarchical_PARMA_FILE_H

#include "PARMA.h"

class HierarchicalPARMA : public PARMA
{
 public:
  virtual double executePARMA();

  static void* func_thread(void*);
};

#endif
