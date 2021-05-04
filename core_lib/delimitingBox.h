#ifndef DELIMITING_BOX_H
#define DELIMITING_BOX_H

#define NUMBER_OF_VECTORS 46

#include <string>

struct idxXYZ;

struct Bound
{
  double lower;
  double upper;
};

class delimitingBox
{
 public:
  
  Bound bounds[NUMBER_OF_VECTORS];
  
  delimitingBox();
  
  void addTuple(struct idxXYZ&);

  void unionWithAnotherBox(delimitingBox);

  void extendMargin(double);

  void deactivate();
  
  void print();

  void importBoxFromStream(std::ifstream&, bool aggregateType = false);
  
  static bool overlap(delimitingBox, delimitingBox);

  std::string serializeTheDelimitingBox(); /*TODO*/
  delimitingBox(char*); /*TODO: constructor for deserialization*/

};


#endif
