#ifndef PARMA_MAP_H
#define PARMA_MAP_H

#include "map.h"
#include "ParmaEllipsoid.h"
#include "ParmaObject.h"

class ParmaMap : public envMap
{
 public:
  ParmaMap();
  
  ParmaObject* getNextParmaObject();
  void importFromFile(char*, bool hasDB = 0);
  
  virtual void combine(envMap&);
  virtual std::vector<Object*>* getObjects();
};
#endif
