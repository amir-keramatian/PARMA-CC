#include "idxXYZ.h"

double innerProduct(idxXYZ p1, idxXYZ p2){
  double result = 0;

  result = (p1.x * p2.x) + (p1.y * p2.y) + (p1.z * p2.z);
  return result;
}

idxXYZ::idxXYZ(double x, double y, double z)
{
  this->x = x;
  this->y = y;
  this->z = z;

  this->clusterIDX = UNASSIGNED;
  this->clusterIDX_beforeCombine = UNASSIGNED;
  this->index_ready = false;
}


/*TODO: KERNEL */
