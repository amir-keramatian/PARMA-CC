#include "ParmaConcurrentMap.h"
#include <string.h>

ParmaConcurrentMap::ParmaConcurrentMap()
{
  type  = "PARMA CONCURRENT MAP";
  ready = false;
}

void ParmaConcurrentMap::combine(envMap& otherMap)
{
  for (int i = 0; i < this->ellipsoids.size(); i++)
    {
      for (int j = 0; j < otherMap.ellipsoids.size(); j++)
	{
	  assert(this->ellipsoids[i]->source != otherMap.ellipsoids[j]->source);/*debug assert*/
	  
	  if (this->ellipsoids[i]->isLinked(otherMap.ellipsoids[j]) == true)
	    {
	      ; // do nothing
	    }
	  else if (this->ellipsoids[i]->overlap(otherMap.ellipsoids[j]) == true)
	    {
	      this->ellipsoids[i]->link(otherMap.ellipsoids[j]);
	    }
	}
    }
}

void ParmaConcurrentMap::importFromFile(char* fileName, bool hasDB)
{
  assert(this->ellipsoids.size() == 0);
  confidenceStep = 1;
  aura = false;
  
  std::ifstream stream(fileName);
  
  char* controlVar = new char[50];
  int numberOfObjects, numberOfEllipsoids;
  
  stream >> controlVar >> numberOfObjects;
  assert(strcmp(controlVar, "#objects:") == 0);

  for (int i = 0; i < numberOfObjects; i++)
    {
      stream >> controlVar >> numberOfEllipsoids;
      assert(strcmp(controlVar, "#ellipsoids:") == 0);
      
      ParmaConcurrentEllipsoid* root = new ParmaConcurrentEllipsoid(-1);

      root->importEllipsoidFromStream(stream, hasDB);
      root->parent = root;
      this->insertEllipsoid(root);
      
      for (int j = 1; j < numberOfEllipsoids; j++)
	{
	  ParmaConcurrentEllipsoid* ellipsoid = new ParmaConcurrentEllipsoid(-1);
	  ellipsoid->parent = root;
	  ellipsoid->importEllipsoidFromStream(stream, hasDB);
	  
	  this->insertEllipsoid(ellipsoid);
	}
    }

  stream.close();
}

std::vector<Object*>* ParmaConcurrentMap::getObjects()
{
  return NULL;
}

void ParmaConcurrentMap::print()
{
  for (int i = 0; i < this->ellipsoids.size(); i++)
    {
      std::cout << "Ellipsoid id = " << ellipsoids[i]->id
		<< " with source = " << ellipsoids[i]->source
		<< " has parent id = " << ellipsoids[i]->findRoot()->id
		<< " and parent source = " << ellipsoids[i]->findRoot()->source
		<< ".\n";
    }
}


