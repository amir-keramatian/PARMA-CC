#include "map.h"
#include "Ellipsoid.h"
#include "combine_and_linkage_stats.h"

#include <sstream>
#include <string.h>
#include "idxXYZ.h"
#include <omp.h>

void envMap::insertEllipsoid(Ellipsoid* toBeInserted)
{
  ellipsoids.push_back(toBeInserted);
  if (toBeInserted->finalized == false)
    {
      printf("Ellipsoid not finalized.\n");
      exit(EXIT_FAILURE);
    }
}

void envMap::resetTheFlags()
{
  std::vector<Ellipsoid*>::iterator ellipsoidIterator = ellipsoids.begin();

  while(ellipsoidIterator != ellipsoids.end())
    {
      (*ellipsoidIterator)->processed = false;
      ellipsoidIterator++;
    }
  ellipsoid_iterator = ellipsoids.begin();
}

Ellipsoid* envMap::getEllipsoidByIDX(int id){

  Ellipsoid* result = NULL;
  
  std::vector<Ellipsoid*>::iterator currentMapIterator;

  for( currentMapIterator = ellipsoids.begin(); currentMapIterator != ellipsoids.end(); currentMapIterator++)
    {
      if ( (*currentMapIterator)->id == id)
	{
	  result = (*currentMapIterator);
	}
    }
  assert(result != NULL);
  return result;
}

void envMap::print()
{
  std::vector<Object*>* objects = getObjects();

  std::cout << "The map's type is " << this->type << ". The map has "  << objects->size() << " objects:" << std::endl;

  for (int i = 0; i < objects->size(); i++)
    {
      std::cout << "\tObject " << i << ":\n";
      ((*objects)[i])->print();
    }
  
}
