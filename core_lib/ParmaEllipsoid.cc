#include "ParmaEllipsoid.h"

ParmaEllipsoid::ParmaEllipsoid(int gid):Ellipsoid(gid)
{
  this->next = this;
}


void ParmaEllipsoid::link(Ellipsoid* e)
{
  /* 1. set the new parent pointer */
  /* 2. expand the bounding box for the new parent */
  /* 3. make a singular linked list */

  if (this->isLinked(e) == true)
    {
      /*Do nothing because they are already linked*/
      return; 
    }
  else
    {
      Ellipsoid *parent_this, *parent_e;

      /* OPTIMIZE WITH RANK ... BLAH BLAH*/
      parent_this = this->findRoot();
      parent_e = e->findRoot();

      ParmaEllipsoid* newRoot = (ParmaEllipsoid*)parent_e;
      ParmaEllipsoid* child = (ParmaEllipsoid*)parent_this;
    
      child->parent = newRoot;

      /* expand bounding boxes */

      newRoot->aggregateDB.unionWithAnotherBox(child->aggregateDB);

      /* CIRCULAR LINKED LIST */
      ParmaEllipsoid* temp = newRoot->next;
      newRoot->next = child->next;
      child->next = temp;
    }
}

void ParmaEllipsoid::finalize()
{
  Ellipsoid::finalize();

  this->aggregateDB = db;
}

Ellipsoid* ParmaEllipsoid::findRoot()
{
  if (this == parent)
    return this;
  
  //path shortcuting:
  this->parent = parent->findRoot();

  return this->parent;
}
