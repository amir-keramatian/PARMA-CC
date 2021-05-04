#ifndef STONE_H
#define STONE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_blas.h>

#include "delimitingBox.h"
#include "idxXYZ.h"
#include "../globals/globals.h"

class Ellipsoid
{
 public:
  Ellipsoid* parent;
  int id, n, source, clusteringClustering_baseline;
  bool finalized, processed; /*The finalized flag shows if mu, cov, characterisitc_matrix and its inverse are calculated and are up-to-date.*/

  double runningSum[3];
  double runningCov[3][3];

  double*  mu;
  double** cov;
  
  gsl_matrix *characteristic_matrix;
  gsl_matrix *characteristic_matrix_inverse;

  delimitingBox db;
  
  Ellipsoid(int);
  
  void addTuple(idxXYZ &);

  void computeMuCov();
  void computeCharacteristicMatrix();
  
  bool isLinked(Ellipsoid*);
  bool overlap(Ellipsoid*);
  char* toString(char*);
  void importEllipsoidFromStream(std::ifstream&, bool hasDB = false);
  void print();

  virtual void finalize();
  virtual Ellipsoid* findRoot() = 0;
  virtual void link(Ellipsoid*) = 0;
};

char* MatrixToString(gsl_matrix*, int);

#endif
