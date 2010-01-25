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

  if (this->getDimAt(1) != rhs.getDimAt(0))
    throw ExeException("Dimension mismatch in matrix multiply");

  int dims[2];
  dims[0] = this->getDimAt(0);
  dims[1] = rhs.getDimAt(1);
  Matrix *result = new Matrix(NULL, 2, dims[0], dims[1]);  

#if 0
  int i = 0;
  for (int x = 0; x != this->getDimAt(0); x++)
    for (int y = 0; y != rhs.getDimAt(1); y++)
      {
        OM_SUPPORT_TYPE element = 0.0;
        for (int z = 0; z != this->getDimAt(1); z++)
          {
            element += this->getElementAt(x, z) * rhs.getElementAt(z, y);
          }
        result->setElementAt(x, y, element);
      }
#else

  cublasSgemm('n', 'n', getDimAt(0), rhs.getDimAt(1), getDimAt(1), 1.0, getDevicePtr(), getDimAt(0), rhs.getDevicePtr(), rhs.getDimAt(0), 0.0, const_cast<OM_SUPPORT_TYPE *>(result->getDevicePtr()), result->getDimAt(0));

  assert(cublasGetError() == CUBLAS_STATUS_SUCCESS);

  result->syncFromDevice(); 

#endif

  // TODO do the multiplication!!!
  //  result->setScalaValue(3.14);
  printf("do the mulplication.\n");
  return result;
}

 void Matrix::streamOut(stringstream &out)
 {
   out << "<br>" << endl <<  getName() << " =" << "<br>"  << endl;
   streamAtom(out);
 }

} // namespace
