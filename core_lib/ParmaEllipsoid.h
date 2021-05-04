#ifndef PARMA_ELLIPSOID_H 
#define PARMA_ELLIPSOID_H 

#include "Ellipsoid.h"

class ParmaEllipsoid : public Ellipsoid
{
 public:
  ParmaEllipsoid(int);

  ParmaEllipsoid* next;
  delimitingBox aggregateDB;

  virtual void finalize();
  virtual Ellipsoid* findRoot();
  virtual void link(Ellipsoid*);
};


#endif
