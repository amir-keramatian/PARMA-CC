#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H

#include "Ellipsoid.h"
#include <vector>

class Object
{
 public:

  virtual bool isSimilar(Object*) = 0;
  
  virtual bool isLinked(Object*) = 0;

  virtual void link(Object*) = 0;

  virtual void print() = 0;

  virtual std::vector<Ellipsoid*>* getEllipsoids() = 0;
};

#endif
