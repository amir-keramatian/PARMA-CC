#ifndef FLEXI_FLAT_PARMA_FILE_H
#define FLEXI_FLAT_PARMA_FILE_H

#include "PARMA.h"

class FlexiFlatPARMA : public PARMA
{
 public:
  virtual double executePARMA();

  static void* func_thread(void*);
  static void* performLocalRelabelingTasks(void*);
};

#endif
