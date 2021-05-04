#include "ParmaConcurrentEllipsoid.h"

ParmaConcurrentEllipsoid::ParmaConcurrentEllipsoid(int gid):Ellipsoid(gid)
{
}


Ellipsoid* ParmaConcurrentEllipsoid::findRoot()
{
  Ellipsoid* parent = this->parent;
  //ParmaConcurrentEllipsoid* parent = (ParmaConcurrentEllipsoid*)this->parent;

  if (parent == this){
    return this;
  }
  return (parent->findRoot());
}

void ParmaConcurrentEllipsoid::link(Ellipsoid* s2)
				    
{
  ParmaConcurrentEllipsoid* parent_of_this = (ParmaConcurrentEllipsoid*)this->findRoot();
  ParmaConcurrentEllipsoid* parent_of_s2   = (ParmaConcurrentEllipsoid*)s2->findRoot();

  if (parent_of_this->id == parent_of_s2->id)
    {
      assert(parent_of_this == parent_of_s2); // debug assert
      return;
    }

  if (parent_of_this->id < parent_of_s2->id)
    {
      if (__sync_bool_compare_and_swap (&(parent_of_this->parent), parent_of_this, parent_of_s2)==true)
	{
	  return;
	}
    }

  if (parent_of_this->id > parent_of_s2->id)
    {
      if (__sync_bool_compare_and_swap (&(parent_of_s2->parent), parent_of_s2, parent_of_this)==true)
	{
	  return;
	}
    }

  parent_of_this->link(parent_of_s2);
}

