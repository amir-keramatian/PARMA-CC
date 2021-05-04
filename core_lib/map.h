#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Ellipsoid.h"
#include "Object.h"

class envMap
{
 public:

  char* type;
  bool ready;
  std::vector<Ellipsoid*> ellipsoids;
  std::vector<Ellipsoid*>::iterator ellipsoid_iterator;
  
  void insertEllipsoid(Ellipsoid*);
  void resetTheFlags();
  Ellipsoid* getEllipsoidByIDX(int);
    
  virtual void print();
  virtual void combine(envMap&) = 0;
  virtual std::vector<Object*>* getObjects() = 0;
};

#endif
