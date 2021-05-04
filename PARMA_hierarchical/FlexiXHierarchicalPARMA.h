#ifndef MP_PARMA_FILE_H
#define MP_PARMA_FILE_H

#include "PARMA.h"

class FlexiXHierarchicalPARMA : public PARMA
{
 public:
  virtual double executePARMA();
  
  static void* func_thread(void*);
  static void* performSmartHierarchicalCombineTasks(void*);
  static void* performLocalRelabelingTasks(void*);
};

inline int minIndexInLeftSubtree(int);
inline int minIndexInRighttSubtree(int);

#endif
