#ifndef IDXXYZ_H
#define IDXXYZ_H

#define UNASSIGNED -1

struct idxXYZ
{
  double x,y,z;
  int clusterIDX;
  int clusterIDX_beforeCombine;
  bool index_ready;

  idxXYZ(double, double, double);
};

double innerProduct(idxXYZ p1, idxXYZ p2);

#endif
