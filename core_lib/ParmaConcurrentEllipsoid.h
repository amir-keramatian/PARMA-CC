#ifndef PARMA_CONCURRENT_ELLIPSOID_H 
#define PARMA_CONCURRENT_ELLIPSOID_H 

#include "Ellipsoid.h"

class ParmaConcurrentEllipsoid : public Ellipsoid
{
 public:
  virtual Ellipsoid* findRoot();
  virtual void link(Ellipsoid*);

  ParmaConcurrentEllipsoid(int);
};

#endif
