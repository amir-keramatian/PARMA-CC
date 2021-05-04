#include "ParmaMap.h"
#include <string.h>

ParmaMap::ParmaMap()
{
  type  = "PARMA MAP";
  ready = false;
}

void ParmaMap::combine(envMap& otherMap)
{
  std::vector<ParmaObject*>* thisMapObjects = (std::vector<ParmaObject*>*) getObjects();
  std::vector<ParmaObject*>* thatMapObjects = (std::vector<ParmaObject*>*)(otherMap.getObjects());

  //std::cout << thisMapObjects->size() << " objects & " << thatMapObjects->size() << " objects\n";
  
  std::vector<ParmaObject*>::iterator thisMapObjectIter = thisMapObjects->begin();
  
  while(thisMapObjectIter != thisMapObjects->end())
    {
      std::vector<ParmaObject*>::iterator thatMapObjectIter = thatMapObjects->begin();

      while(thatMapObjectIter != thatMapObjects->end())
	{
	  if( (*thisMapObjectIter)->isLinked(*thatMapObjectIter) == true )
	    {
	      ; // do nothing
	    }
	  else if ( (*thisMapObjectIter)->isSimilar(*thatMapObjectIter) == true )
	    {
	      (*thisMapObjectIter)->link(*thatMapObjectIter);
	    }
	  thatMapObjectIter++;
	}
      thisMapObjectIter++;
    }
  ellipsoids.insert(ellipsoids.end(), otherMap.ellipsoids.begin(), otherMap.ellipsoids.end());
}

std::vector<Object*>* ParmaMap::getObjects()
{
  resetTheFlags();
  std::vector<Object*> *result = new std::vector<Object*>();

  ParmaObject* nextObject = getNextParmaObject();
  while(nextObject->representative != NULL)
    {
      result->push_back((Object*)nextObject);
      nextObject = getNextParmaObject();
    }
  
  return result;
}

ParmaObject* ParmaMap::getNextParmaObject()
{
  ParmaObject* nextObject = new ParmaObject();
  if (ellipsoid_iterator == ellipsoids.end())
    {
      nextObject->representative = NULL;
      return nextObject;
    }

  nextObject->representative = (ParmaEllipsoid*)((*ellipsoid_iterator)->findRoot());
  
  if (nextObject->representative->processed == false)
    {
      nextObject->representative->processed = true;
      ellipsoid_iterator++;
      return nextObject;
    }
  else
    {
      ellipsoid_iterator++;
      return getNextParmaObject();
    }
}

void ParmaMap::importFromFile(char* fileName, bool hasDB)
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
      
      ParmaEllipsoid* root = new ParmaEllipsoid(-1);

      if (hasDB == true)
	{
	  root->aggregateDB.importBoxFromStream(stream, true);
	}
      else
	{
	  root->aggregateDB.deactivate();
	}

      root->importEllipsoidFromStream(stream, hasDB);
      root->parent = root;
      root->next = root;
      this->insertEllipsoid(root);
      
      ParmaEllipsoid* prev = root;
      
      for (int j = 1; j < numberOfEllipsoids; j++)
	{
	  ParmaEllipsoid* ellipsoid = new ParmaEllipsoid(-1);
	  ellipsoid->aggregateDB.deactivate();
	  ellipsoid->parent = root;
	  ellipsoid->next = root;
	  ellipsoid->importEllipsoidFromStream(stream, hasDB);
	  
	  this->insertEllipsoid(ellipsoid);
	  
	  prev->next = ellipsoid;
	  prev = ellipsoid;
	}

    }

  stream.close();
}
