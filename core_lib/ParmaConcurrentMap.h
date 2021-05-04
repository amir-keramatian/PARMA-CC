#ifndef PARMA_CONCURRENT_MAP_H
#define PARMA_CONCURRENT_MAP_H

#include "ParmaConcurrentEllipsoid.h"
#include "map.h"

class Object;

class ParmaConcurrentMap : public envMap
{
 public:
  ParmaConcurrentMap();

  virtual void combine(envMap&);

  void importFromFile(char*, bool hasDB = 0);

  virtual void print();
  
  virtual std::vector<Object*>* getObjects();
};

#endif

