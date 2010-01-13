#include "Matrix.h"
#include "assert.h"
namespace ONLINE_MATLAB{


  /*Matrix :: Matrix():
  Atom()
  {}*/

  Matrix :: Matrix(int _dim, ...):
  Atom(_dim),
    mNext(0)
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

Matrix :: Matrix(int dim, int *dims, OM_SUPPORT_TYPE *elements):
  Atom(dim, dims, elements),
    mNext(0)
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

  assert(this->getDimAt(1) == rhs.getDimAt(0));
  int dims[2];
  dims[0] = this->getDimAt(0);
  dims[1] = this->getDimAt(1);
  Matrix *result = new Matrix(2, dims[0], dims[1]);  

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

        
      
  // TODO do the multiplication!!!
  //  result->setScalaValue(3.14);
  printf("do the mulplication.\n");
  return result;
}

}
