#ifndef PARMA_OBJECT_H
#define PARMA_OBJECT_H

#include "Object.h"
#include "ParmaEllipsoid.h"
class ParmaObject : public Object
{
 public:
  ParmaEllipsoid* representative = NULL;

  virtual bool isSimilar(Object*);
  
  virtual bool isLinked(Object*);

  virtual void link(Object*);

  virtual void print();

  virtual std::vector<Ellipsoid*>* getEllipsoids();
};

#endif
