#include "delimitingBox.h"
#include "idxXYZ.h"
#include <limits.h>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <string.h>

idxXYZ vectors[NUMBER_OF_VECTORS] = {
  {1.0000, 0.0000, 0.0000},
  {0.9211, 0.3894, 0.0000},
  {0.6967, 0.7174, 0.0000},
  {0.3624, 0.9320, 0.0000},
  {-0.0292, 0.9996, 0.0000},
  {-0.4161, 0.9093, 0.0000},
  {-0.7374, 0.6755, 0.0000},
  {-0.9422, 0.3350, 0.0000},
  {-1.0000, 0.0000, 0.0000},
  {0.9798, 0.0000, 0.2000},
  {0.9025, 0.3816, 0.2000},
  {0.6826, 0.7029, 0.2000},
  {0.3550, 0.9132, 0.2000},
  {-0.0286, 0.9794, 0.2000},
  {-0.4077, 0.8909, 0.2000},
  {-0.7225, 0.6618, 0.2000},
  {-0.9232, 0.3282, 0.2000},
  {-0.9798, 0.0000, 0.2000},
  {0.9165, 0.0000, 0.4000},
  {0.8442, 0.3569, 0.4000},
  {0.6385, 0.6575, 0.4000},
  {0.3321, 0.8542, 0.4000},
  {-0.0268, 0.9161, 0.4000},
  {-0.3814, 0.8334, 0.4000},
  {-0.6758, 0.6191, 0.4000},
  {-0.8636, 0.3070, 0.4000},
  {-0.9165, 0.0000, 0.4000},
  {0.8000, 0.0000, 0.6000},
  {0.7368, 0.3115, 0.6000},
  {0.5574, 0.5739, 0.6000},
  {0.2899, 0.7456, 0.6000},
  {-0.0234, 0.7997, 0.6000},
  {-0.3329, 0.7274, 0.6000},
  {-0.5899, 0.5404, 0.6000},
  {-0.7538, 0.2680, 0.6000},
  {-0.8000, 0.0000, 0.6000},
  {0.6000, 0.0000, 0.8000},
  {0.5526, 0.2337, 0.8000},
  {0.4180, 0.4304, 0.8000},
  {0.2174, 0.5592, 0.8000},
  {-0.0175, 0.5997, 0.8000},
  {-0.2497, 0.5456, 0.8000},
  {-0.4424, 0.4053, 0.8000},
  {-0.5653, 0.2010, 0.8000},
  {-0.6000, 0.0000, 0.8000},
  {0.0000, 0.0000, 1.0000}
};


delimitingBox::delimitingBox()
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      bounds[i].lower = INT_MAX;
      bounds[i].upper = INT_MIN;
    }
}

void delimitingBox::addTuple(struct idxXYZ& tuple)
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      double innnerProduct_result = innerProduct(vectors[i], tuple);

      if (innnerProduct_result < bounds[i].lower){
	bounds[i].lower = innnerProduct_result;
      }
    
      if (innnerProduct_result > bounds[i].upper){
	bounds[i].upper = innnerProduct_result;
      }
    }
}

void delimitingBox::deactivate()
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      bounds[i].lower = INT_MIN;
      bounds[i].upper = INT_MAX;
    }
}


void delimitingBox::print()
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      std::cout << "projection on vector[" << i << "]: lower bound = " << bounds[i].lower << " upper bound = " << bounds[i].upper << std::endl;
    }

}

bool delimitingBox::overlap(delimitingBox db1, delimitingBox db2)
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      if (db1.bounds[i].upper < db2.bounds[i].lower)
	return false;
      
      if (db2.bounds[i].upper < db1.bounds[i].lower)
	return false;
    }
  
  return true;
}

void delimitingBox::extendMargin(double delta)
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++){

    assert(this->bounds[i].lower != INT_MAX); /*make sure bounds are inialized properly*/
    assert(this->bounds[i].upper != INT_MIN); /*make sure bounds are inialized properly*/
    
    this->bounds[i].lower -= delta;
    this->bounds[i].upper += delta;
  }
}

void delimitingBox::unionWithAnotherBox(delimitingBox anotherBox)
{
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {

      assert(this->bounds[i].lower != INT_MAX); /*make sure bounds are inialized properly*/
      assert(this->bounds[i].upper != INT_MIN); /*make sure bounds are inialized properly*/

      assert(anotherBox.bounds[i].lower != INT_MAX); /*make sure bounds are inialized properly*/
      assert(anotherBox.bounds[i].upper != INT_MIN); /*make sure bounds are inialized properly*/
    
      this->bounds[i].lower = fmin(this->bounds[i].lower, anotherBox.bounds[i].lower);
      this->bounds[i].upper = fmax(this->bounds[i].upper, anotherBox.bounds[i].upper);
    }
}

std::string delimitingBox::serializeTheDelimitingBox(){
  /***********TODO********/

  return std::string("TODO!");
}

void delimitingBox::importBoxFromStream(std::ifstream & stream, bool aggregateType)
{
  char* controlVar = new char[50];

  stream >> controlVar;

  if (aggregateType == false)
    {
      assert(strcmp(controlVar, "@delimitingBox:") == 0);
    }
  else
    {
      assert(strcmp(controlVar, "@aggregateDelimitingBox:") == 0);
    }
  
  for (int i = 0; i < NUMBER_OF_VECTORS; i++)
    {
      stream >> this->bounds[i].lower;
      stream >> this->bounds[i].upper;
    }
  
}
