#include "Matrix.h"
#include "assert.h"
#include <iostream>
#include <cublas.h>

#include "../include/ExeException.h"

namespace ONLINE_MATLAB{

using namespace std;

  Matrix :: Matrix(const char *name, int _dim, ...):
  Atom(_dim),
    mNext(0),
    mName(name)
{

  int dims[_dim];

  va_list ap;
  va_start(ap, _dim);
  for(int i=0; i<_dim; i++){
    dims[i] = va_arg(ap, int);
  }

  setDims(_dim, dims);

  printf("%p in matrix: dim = %d.\n", this, getDim());
  //  mHostBuffer = (void *)calloc(length, sizeof(OM_SUPPORT_TYPE));
}

  Matrix :: Matrix(const char *name, int dim, int *dims, OM_SUPPORT_TYPE *elements):
  Atom(dim, dims, elements),
    mNext(0),
    mName(name)
{
}
  /*Matrix :: Matrix():
  length(0),
  dim(0),
  Atom(0)
  {}*/

Matrix :: ~Matrix(){
}


/* Matrix::Matrix(const Matrix &rhs): // copy constructor
  Atom(dim, dims, elements),
    mNext(0),
    mName(name)

  {
    
  }*/

/*int Matrix :: getDim(){
  return dim;
}*/

void Matrix :: allocMatrix(){
  // TODO 
  /*  if(mHostBuffer == NULL)
    mHostBuffer = (void *)calloc(length, sizeof(OM_SUPPORT_TYPE));
  else
  mHostBuffer = (void *)realloc(mHostBuffer, length);*/
}

//TODO: throw exception when dims do not match
Matrix *Matrix :: operator+(Matrix const &rhs) const
{
	printf("in operator +\n");
	assert(this->getDimAt(0)==rhs.getDimAt(0));
	assert(this->getDimAt(1)==rhs.getDimAt(1));

	int dims[2];
	dims[0] = this->getDimAt(0);
	dims[1] = this->getDimAt(1);
	Matrix *result = new Matrix(NULL, 2, dims[0], dims[1]);
	for(int i=0; i<dims[0]; i++){
		for(int j=0; j<dims[1]; j++){
			result->setElementAt(i, j, this->getElementAt(i, j) + rhs.getElementAt(i, j));
		}
	}

	result->syncToDevice();
    result->setInitialized(true);
	return result;

}

//TODO: throw exception when dims do not match
Matrix *Matrix :: operator-(Matrix const &rhs) const
{
	printf("in operator -\n");
	assert(this->getDimAt(0)==rhs.getDimAt(0));
	assert(this->getDimAt(1)==rhs.getDimAt(1));

	int dims[2];
	dims[0] = this->getDimAt(0);
	dims[1] = this->getDimAt(1);
printf("addition1\n");
	Matrix *result = new Matrix(NULL, 2, dims[0], dims[1]);
	for(int i=0; i<dims[0]; i++){
		for(int j=0; j<dims[1]; j++){
			result->setElementAt(i, j, this->getElementAt(i, j) - rhs.getElementAt(i, j));
		}
	}

	result->syncToDevice();
    result->setInitialized(true);
	return result;

}


Matrix *Matrix :: operator*(Matrix const &rhs) const
{

   printf("before do the mulplication.\n");
   assert(&rhs != 0);
  int ldim = this->getDim();
  int rdim = rhs.getDim();
  
  
  assert(ldim == 2);
  assert(rdim == 2);

  assert(this->initialized());
  assert(rhs.initialized());

  Matrix *result;

  if (this->getDimAt(0) == this->getDimAt(1) && this->getDimAt(1) == 1)
    {
      result = new Matrix(NULL, 2, rhs.getDimAt(0), rhs.getDimAt(1));
      result->syncToDevice();
      cublasSaxpy(rhs.getBufferSize(), getScalaValue(), rhs.getDevicePtr(), 1, const_cast<OM_SUPPORT_TYPE *>(result->getDevicePtr()), 1);
      if (cublasGetError() != CUBLAS_STATUS_SUCCESS){
		throw ExeException("Internal error during multiplication.");
      }
      result->syncFromDevice(); 
    } 
  else if(rhs.getDimAt(0) == rhs.getDimAt(1) && rhs.getDimAt(1) == 1)
    {
      result = new Matrix(NULL, 2, getDimAt(0), getDimAt(1));
      result->syncToDevice();
      cublasSaxpy(this->getBufferSize(), rhs.getScalaValue(), this->getDevicePtr(), 1, const_cast<OM_SUPPORT_TYPE *>(result->getDevicePtr()), 1);
      if (cublasGetError() != CUBLAS_STATUS_SUCCESS){
		throw ExeException("Internal error during multiplication.");
      }
      result->syncFromDevice(); 
    }
  else {
    if (this->getDimAt(1) != rhs.getDimAt(0))
      throw ExeException("Dimension mismatch in matrix multiply");
    
    int dims[2];
    dims[0] = this->getDimAt(0);
    dims[1] = rhs.getDimAt(1);
    result = new Matrix(NULL, 2, dims[0], dims[1]);  
    
    
    cublasSgemm('n', 'n', getDimAt(0), rhs.getDimAt(1), getDimAt(1), 1.0, getDevicePtr(), getDimAt(0), rhs.getDevicePtr(), rhs.getDimAt(0), 0.0, const_cast<OM_SUPPORT_TYPE *>(result->getDevicePtr()), result->getDimAt(0));
    
    assert(cublasGetError() == CUBLAS_STATUS_SUCCESS);
    
    result->syncFromDevice(); 
    
  }

  result->setInitialized(true);
  return result;
}

 Matrix *Matrix::transpose()
 {
   if (this->getDim() != 2)
     throw ExeException("Matrix transpose must be two-dimensional.");

   Matrix *m = new Matrix(NULL, 2, this->getDimAt(1), this->getDimAt(0));

   for (int i = 0; i != getDimAt(1); i++)
     for (int j = 0; j != getDimAt(0); j++)
       m->setElementAt(i,j, this->getElementAt(j,i));
   
   m->syncToDevice();
   m->setInitialized(true);
   return m;
 }

 void Matrix::streamOut(stringstream &out)
 {
   out << "<br>" << endl <<  getName() << " =" << "<br>"  << endl;
   streamAtom(out);
 }

} // namespace
