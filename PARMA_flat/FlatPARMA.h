#ifndef FLAT_PARMA_FILE_H
#define FLAT_PARMA_FILE_H

#include "PARMA.h"

class FlatPARMA : public PARMA
{
 public:
  virtual double executePARMA();

  static void* func_thread(void*);
};

#endif
