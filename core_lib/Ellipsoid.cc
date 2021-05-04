#include "Ellipsoid.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <string.h>
#include "combine_and_linkage_stats.h"

using namespace std;

Ellipsoid::Ellipsoid(int gid) 				
{
  parent = this;
  id     = gid;
  n      = 0;
  source = -999;
  clusteringClustering_baseline = -1;
  finalized = false;
  processed = false;

  for (int i = 0; i < 3; i++)
    {
      runningSum[i] = 0.0;
      
      for (int j = 0; j < 3; j++)
	{
	  runningCov[i][j] = 0.0;
	}
    }
  
  mu  = NULL;
  cov = NULL;
  characteristic_matrix = NULL;
  characteristic_matrix_inverse = NULL;
}

void Ellipsoid::addTuple(struct idxXYZ & tuple)
{
  finalized = false;
  
  if (tuple.clusterIDX != id || tuple.clusterIDX == 0)
    {
      printf("Error in initializing the Ellipsoid.\n");
      exit(EXIT_FAILURE);
    }

  this->db.addTuple(tuple);
  
  n++;

  double dataPoint[3] = {tuple.x, tuple.y, tuple.z};
  
  for (int i = 0; i < 3; ++i)
    {
      runningSum[i] += dataPoint[i];

      for (int j = 0; j < 3; ++j)
	{
	  runningCov[i][j] += dataPoint[i] * dataPoint[j];
	}
    }
}

void Ellipsoid::computeMuCov()
{
  if(finalized == true)
    return;

  if ( n == 0)
    {
      printf("Division by zero in the computeMuCov fucntion.\n");
      exit(EXIT_FAILURE);
    }

  assert (mu  == NULL); /*In a streaming version these asserts can be removed.*/
  assert (cov == NULL); /*In a streaming version these asserts can be removed.*/
  
  mu  = new double[3];
  cov = new double*[3];
  
  for (int i = 0; i < 3; ++i)
    {
      cov[i] = new double[3];
    }
  
  mu[0] = runningSum[0]/n;
  mu[1] = runningSum[1]/n;
  mu[2] = runningSum[2]/n;
  
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
	{
	  cov[i][j] = confidenceStep*confidenceStep*(runningCov[i][j]/n - mu[i]*mu[j]);
	}
      cov[i][i] += 0.01;
    }

  return;
}

bool Ellipsoid::isLinked(Ellipsoid* e)
{
  assert(this->finalized);
  assert(e->finalized);
  
  Ellipsoid *parent_this, *parent_e;

  parent_this = this->findRoot();
  parent_e    = e->findRoot();

  return (parent_this == parent_e);  
}

bool Ellipsoid::overlap(Ellipsoid* e)
{
  assert(this->finalized);
  assert(e->finalized);

  if (this->source == e->source) /*Ellipsoids from the same source are not overlapping*/
    {
      return false;
    }


  if (delimitingBox::overlap(this->db, e->db) == false)
    {
      return false;
    }
  
  if (ellipsoidalModeling == false)
    {
      assert(delimitingBox::overlap(this->db, e->db) == true);
      return true;
    }
  
  bool overlapOrTouch = false;

  gsl_matrix* decisionMatrix = gsl_matrix_alloc(4,4);

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
		 1.0, this->characteristic_matrix_inverse, e->characteristic_matrix,
		 0.0, decisionMatrix);
  /*
  std::cout << this->id << " & " << e->id
	    << "\nmu_1\n\t" << this->toString("mu") << "\ncov_1\n\t" << this->toString("cov")
	    << "\ncharacteristic matrix\n\t" << MatrixToString(this->characteristic_matrix, 4)
	    << "\nmu_2\n\t" << e->toString("mu") << "\ncov_2\n\t" << e->toString("cov")
    	    << "\ncharacteristic matrix\n\t" << MatrixToString(e->characteristic_matrix, 4)
	    << std::endl
	    << "Decision matrix:\n\t"
	    << MatrixToString(decisionMatrix, 4) << std::endl;
  */
  
  gsl_vector_complex *eval = gsl_vector_complex_alloc(4);
  gsl_matrix_complex *evec = gsl_matrix_complex_alloc(4,4);

  gsl_eigen_nonsymmv_workspace *w = gsl_eigen_nonsymmv_alloc(4);

  gsl_eigen_nonsymmv(decisionMatrix, eval, evec, w);

  gsl_eigen_nonsymmv_free(w);

  //gsl_eigen_nonsymmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_DESC);

  {
    int i, j;

    for (i = 0; i < 4; i++) //iterate over eigen vectors/values
      {
	gsl_complex eval_i = gsl_vector_complex_get (eval, i);
	gsl_vector_complex_view evec_i = gsl_matrix_complex_column (evec, i);

	//printf ("eigenvalue = %g + %gi\n", GSL_REAL(eval_i), GSL_IMAG(eval_i));
	//printf ("eigenvector = \n");
	
	gsl_matrix* a_vector = gsl_matrix_alloc(4,1);
	gsl_matrix* intermediate_row_vector = gsl_matrix_alloc(1,4);
	gsl_matrix* final = gsl_matrix_alloc(1,1);
	bool admissible = true;
	gsl_complex fourthDimension = gsl_vector_complex_get(&evec_i.vector, 3);


	if ((GSL_REAL(fourthDimension) == 0) && (GSL_IMAG(fourthDimension) == 0))
	  {
	    admissible = false;
	  }

	if (admissible)
	  {
	    for (j = 0; j < 4; ++j)
	      {
		gsl_complex z =
		  gsl_vector_complex_get(&evec_i.vector, j);
		//printf("%g + %gi\n", GSL_REAL(z), GSL_IMAG(z));

		z = gsl_complex_div(z, fourthDimension);
		//#	      a(j) = GSL_REAL(z);

		gsl_matrix_set(a_vector, j, 0, GSL_REAL(z));
	      
		//a(j).imag() = 0;//GSL_IMAG(z);

	      } //end for j = 0; j < 4; j++
	
	    //cout << "X' A X = " <<  (a.transpose()*A)*(a) << " inside decision: " << (( (a.transpose()*A)*(a))  <= 0.00001) << endl;
	    //cout << "X' B X = " <<  (a.transpose()*B)*(a) << " inside decision: " << (( (a.transpose()*B)*(a))  <= 0.00001) << endl;

	    //#	  if (    ( ((a.transpose()*A)*(a))  <= 0.00001 ) &&
	    //#	  (( (a.transpose()*B)*(a))  <= 0.00001 )        ) {


	    gsl_blas_dgemm(CblasTrans, CblasNoTrans,
			   1.0, a_vector, this->characteristic_matrix,
			   0.0, intermediate_row_vector);

	    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
			   1.0, intermediate_row_vector, a_vector,
			   0.0, final);

	    double decision_1 = gsl_matrix_get(final, 0, 0);


	    gsl_blas_dgemm(CblasTrans, CblasNoTrans,
			   1.0, a_vector, e->characteristic_matrix,
			   0.0, intermediate_row_vector);

	    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
			   1.0, intermediate_row_vector, a_vector,
			   0.0, final);

	    double decision_2 = gsl_matrix_get(final, 0, 0);

	    if (    ( decision_1  <= 0.00000001 ) &&
		    ( decision_2  <= 0.00000001 )        ) {

	      overlapOrTouch = true;
	      break;
	    }
	  } // end if admissible
      } //end for i = 0; i < 4; i++
  }

  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);

  //std::cout << "********************" <<overlapOrTouch << std::endl;
  
  return overlapOrTouch;
}

void Ellipsoid::finalize()
{
  if (finalized == true)
    {
      return;
    }

  if (aura == true)
    {
      this->db.extendMargin(EPSILON/2);
    }

  if (ellipsoidalModeling)
    {
      computeMuCov();
      computeCharacteristicMatrix();
    }
  finalized = true;
  
  return;
}

void Ellipsoid::print()
{
  assert(this->finalized);

  std::cout << "cluster " << id << std::endl;
  std::cout << "mu:" << std::endl;
  
  for (int i = 0; i < 3; ++i)
    {
      std::cout << mu[i] << " ";
    }

  std::cout << std::endl;
  std::cout << "sigma:" << std::endl;
  
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
	{
	  std::cout << cov[i][j] << "\t";
	}
      std::cout << std::endl;
    }
  std::cout << std::endl << std::endl;
}

void Ellipsoid::computeCharacteristicMatrix()
{
  if(finalized == true)
    return;

  assert(mu  != NULL);
  assert(cov != NULL);

  gsl_matrix* covariance_matrix = gsl_matrix_alloc(3,3);
  gsl_matrix* lambda_matrix = gsl_matrix_alloc(3,3);
  gsl_matrix* lambda_inverse_matrix = gsl_matrix_alloc(3,3);
  gsl_matrix* product_matrix = gsl_matrix_alloc(3,3);
  gsl_matrix* product_intermediate_matrix = gsl_matrix_alloc(3,3);

  gsl_matrix* T_matrix = gsl_matrix_alloc(4,4);
  gsl_matrix* Sigma_inverse_four_by_four = gsl_matrix_alloc(4,4);
  gsl_matrix* characteristic_intermediate = gsl_matrix_alloc(4,4);
  characteristic_matrix = gsl_matrix_alloc(4,4);

  for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
	{
	  gsl_matrix_set(T_matrix, i, j, 0);
	  gsl_matrix_set(Sigma_inverse_four_by_four, i, j, 0);
	  gsl_matrix_set(characteristic_intermediate, i, j, 0);
	  gsl_matrix_set(characteristic_matrix, i, j, 0);
	}
      gsl_matrix_set(T_matrix, i, i, 1);
    }
  
  gsl_matrix_set(T_matrix, 3, 0, -mu[0]);
  gsl_matrix_set(T_matrix, 3, 1, -mu[1]);
  gsl_matrix_set(T_matrix, 3, 2, -mu[2]);
  //  gsl_matrix_set(T_matrix, 3, 3, 1);

  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
	{
	  gsl_matrix_set(covariance_matrix, i, j, cov[i][j]);
	  gsl_matrix_set(lambda_matrix, i, j, 0);
	  gsl_matrix_set(lambda_inverse_matrix, i, j, 0);
	  gsl_matrix_set(product_matrix, i, j, 0);
	  gsl_matrix_set(product_intermediate_matrix, i, j, 0);
	}
    }

  gsl_vector *eval = gsl_vector_alloc(3);
  gsl_matrix *evec = gsl_matrix_alloc(3,3);

  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(3);

  gsl_eigen_symmv(covariance_matrix, eval, evec, w);

  gsl_eigen_symmv_free(w);

  {
    int i;

    for (i = 0; i < 3; i++)
      {
	double eval_i
	  = gsl_vector_get (eval, i);
	gsl_vector_view evec_i
	  = gsl_matrix_column (evec, i);

	if (aura == true)
	  {
	    eval_i = (sqrt(eval_i) + EPSILON/2)*(sqrt(eval_i) + EPSILON/2);
	  } 
	
	gsl_matrix_set(lambda_matrix, i, i, eval_i);
	gsl_matrix_set(lambda_inverse_matrix, i, i, (1/eval_i));
      }
  }

  /*
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
    1.0, evec, lambda_matrix,
    0.0, product_intermediate_matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans,
    1.0, product_intermediate_matrix, evec,
    0.0, product_matrix);*/

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
		 1.0, evec, lambda_inverse_matrix,
		 0.0, product_intermediate_matrix);
  gsl_blas_dgemm(CblasNoTrans, CblasTrans,
  		 1.0, product_intermediate_matrix, evec,
  		 0.0, product_matrix);

  /*product_matrix contains sigma inverse */

  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
	{
	  gsl_matrix_set(Sigma_inverse_four_by_four, i, j,
			 gsl_matrix_get(product_matrix, i, j)
			 );
	}
    }
  gsl_matrix_set(Sigma_inverse_four_by_four, 3, 3, -1);

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
		 1.0, T_matrix, Sigma_inverse_four_by_four,
		 0.0, characteristic_intermediate);
  gsl_blas_dgemm(CblasNoTrans, CblasTrans,
  		 1.0, characteristic_intermediate, T_matrix,
  		 0.0, characteristic_matrix);
  
  gsl_vector_free (eval);
  gsl_matrix_free (evec);

  gsl_matrix* cm_copy = gsl_matrix_alloc(4, 4);
  for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
	{
	  gsl_matrix_set(cm_copy, i, j,
			 gsl_matrix_get(characteristic_matrix, i, j)
			 );
	}
    }

  
  gsl_permutation *p = gsl_permutation_alloc(4);
  int s;
  gsl_linalg_LU_decomp(cm_copy, p, &s);

  characteristic_matrix_inverse = gsl_matrix_alloc(4, 4);
  gsl_linalg_LU_invert(cm_copy, p, characteristic_matrix_inverse);
  gsl_permutation_free(p);

  /*
  std::cout << "characteristic matrix:\n\t"
	    << toString("characteristic_matrix")
	    << std::endl
	    << "characteristic matrix inverse:\n\t"
	    << toString("characteristic_matrix_inverse") << std::endl;
  */
}

char* Ellipsoid::toString(char* component)
{
  char* result = new char[200];
			  
  if (strcmp(component, "mu") == 0)
    {
      sprintf(result, "[%.3f %.3f %.3f]", mu[0], mu[1], mu[2]);
    }

  else if (strcmp(component, "cov") == 0)
    {
      sprintf(result, "[%.3f %.3f %.3f; %.3f %.3f %.3f; %.3f %.3f %.3f]",
	      cov[0][0], cov[0][1], cov[0][2],
	      cov[1][0], cov[1][1], cov[1][2],
	      cov[2][0], cov[2][1], cov[2][2]
	      );
    }

  else if (strcmp(component, "characteristic_matrix") == 0)
    {
      assert(characteristic_matrix != NULL);

      return MatrixToString(characteristic_matrix, 4);
    }

  else if (strcmp(component, "characteristic_matrix_inverse") == 0)
    {
      assert(characteristic_matrix_inverse != NULL);
      
      return MatrixToString(characteristic_matrix_inverse, 4);
    }
  
  else
    {
      printf("Can not stringify the component.\n");
      exit(EXIT_FAILURE);
    }

  return result;
}

void Ellipsoid::importEllipsoidFromStream(std::ifstream & stream, bool hasDB)
{
  char* controlVar = new char[50];

  stream >> controlVar >> this->id;
  assert(strcmp(controlVar, "@id:") == 0);

  stream >> controlVar >> this->source;
  assert(strcmp(controlVar, "@sourceID:") == 0);

  this->mu = new double[3];
  this->cov = new double*[3];
  for (int k = 0; k < 3; k++)
    {
      this->cov[k] = new double[3];
    }
  
  stream >> controlVar;
  for ( int k = 0; k < 3; k++)
    stream >> this->mu[k];
  assert(strcmp(controlVar, "@mu:") == 0);

  stream >> controlVar;
  for ( int k = 0; k < 3; k++)
    for ( int l = 0; l < 3; l++)
      stream >> this->cov[k][l];
  assert(strcmp(controlVar, "@cov:") == 0);

  this->computeCharacteristicMatrix();

  if (hasDB == true)
    {
      this->db.importBoxFromStream(stream);
    }
  else
    {
      this->db.deactivate();
    }
  
  this->finalized = true;  
}

char* MatrixToString(gsl_matrix* matrix, int size)
{
  char* result = new char[200];
      
  sprintf(result, "[");
  for (int i = 0; i < size; i++)
    {
      for (int j = 0; j < size; j++)
	{
	  sprintf(result, "%s %.3f",
		  result,
		  gsl_matrix_get(matrix, i, j));

	}
      (i != size - 1)? sprintf(result, "%s;", result): NULL;;
    }
  sprintf(result, "%s]", result);
  return result;
}  
