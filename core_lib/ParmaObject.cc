#include "ParmaObject.h"

bool ParmaObject::isSimilar(Object* s)
{
  ParmaObject* s2 = (ParmaObject*) s;

  if (delimitingBox::overlap(this->representative->aggregateDB, s2->representative->aggregateDB) == false)
    return false;

  ParmaEllipsoid* thisEllipsoid = this->representative;
  do
    {
      thisEllipsoid = thisEllipsoid->next;
      ParmaEllipsoid* thatEllipsoid = s2->representative;
      do
	{
	  thatEllipsoid = thatEllipsoid->next;
	  if (thisEllipsoid->overlap(thatEllipsoid) == true)
	    return true;
	} while(thatEllipsoid != s2->representative);

    } while(thisEllipsoid != this->representative);

  return false;
}

bool ParmaObject::isLinked(Object* obj)
{
  return this->representative->isLinked(((ParmaObject*)obj)->representative);
}

void ParmaObject::link(Object* obj)
{
  if (this->isLinked(obj))
    return;
  
  this->representative->link(((ParmaObject*)obj)->representative);
}

void ParmaObject::print()
{
  assert(this->representative != NULL);
  assert(this->representative == this->representative->findRoot());

  std::vector<ParmaEllipsoid*>* ellipsoids
    = (std::vector<ParmaEllipsoid*>*)this->getEllipsoids();

  std::cout << "\t\tThe object's root id is "
	    << this->representative-> id
	    << ". The object contains "
	    << ellipsoids->size()
	    << " ellipsoids:"
	    << std::endl;

  for (int i = 0; i < ellipsoids->size(); i++)
    {
      std::cout << "\t\t\tid: "
		<< ((*ellipsoids)[i])->id
		<< ". mu = ["
		<< ((*ellipsoids)[i])->toString("mu")
		<< "]. cov = ["
		<< ((*ellipsoids)[i])->toString("cov")
		<< "]."
		<< std::endl;
    }

}

std::vector<Ellipsoid*>* ParmaObject::getEllipsoids()
{
  std::vector<Ellipsoid*>* result = new std::vector<Ellipsoid*>();
  
  ParmaEllipsoid* thisEllipsoid = this->representative;
  do
    {
      thisEllipsoid = thisEllipsoid->next;
      
      result->push_back((Ellipsoid*)thisEllipsoid);
      
    } while(thisEllipsoid != this->representative);

  return result;
}
