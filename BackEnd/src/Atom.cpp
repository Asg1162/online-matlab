#include "Atom.h"
#include <cublas.h>

using namespace std;

namespace ONLINE_MATLAB{

    Atom :: Atom(int _dim):
  mInitialized(false),
  mDim(_dim),
  mBufSize(1)
{
  mHostBuffer = 0;
  mDvceBuffer = 0;
}

  Atom :: Atom(int dim, int *dims)
{

  mBufSize = 1;

  for(int i=0; i<dim; i++){
    mDLen.push_back(dims[i]);
    mBufSize *= dims[i];
  }

  mDim = dim;
  printf("%p here in atom.dim = %d\n", this, mDim);
  mInitialized = false;
  mHostBuffer = (OM_SUPPORT_TYPE *)calloc(mBufSize, sizeof(OM_SUPPORT_TYPE));
  cublasStatus status = cublasAlloc(mBufSize, sizeof(OM_SUPPORT_TYPE), (void **)&mDvceBuffer);
  assert(status == CUBLAS_STATUS_SUCCESS);
  /*mBufSize = 1;

  va_list ap;
  va_start(ap, dim);
  for (int i = 0; i < dim; i++)
    mDLen.push_back(va_arg(ap, int));

  va_end(ap); 
  mDim = dim;

  for(vector<int>::iterator it=mDLen.begin(); it!=mDLen.end(); it++){
    mBufSize *= *it;
  } 

  mInitialized = false;
  mHostBuffer = (OM_SUPPORT_TYPE *)calloc(mBufSize, sizeof(OM_SUPPORT_TYPE));
  */
}

Atom :: Atom(int dim, int *dims, OM_SUPPORT_TYPE *elements)
{

  mBufSize = 1;
  
  for(int i=0; i<dim; i++){
    mDLen.push_back(dims[i]);
    mBufSize *= dims[i];
  }

  mDim = dim;
  mInitialized = true; 
  mHostBuffer = (OM_SUPPORT_TYPE *)calloc(mBufSize, sizeof(OM_SUPPORT_TYPE));
  cublasStatus status = cublasAlloc(mBufSize, sizeof(OM_SUPPORT_TYPE), (void **)&mDvceBuffer);
  assert(status == CUBLAS_STATUS_SUCCESS);
  // if dim = 2, convert from row-wise to column-wise
  if (dim == 2)
    {
      OM_SUPPORT_TYPE buffer[mDLen[0]][mDLen[1]];
      OM_SUPPORT_TYPE *bufferd = &buffer[0][0];
      for (int i = 0; i < mBufSize; i++)
        bufferd[i] = elements[i];
      // transpose
      int index = 0;
      for (int i = 0; i < mDLen[1]; i++)
        for (int j = 0; j < mDLen[0]; j++)
          mHostBuffer[index++] = buffer[j][i];
    }
  else // else use row-wise
    {
      for (int i = 0; i < mBufSize; ++i)
        {
          mHostBuffer[i] = elements[i];
        }
    }

  // sync with device buffer
  cublasSetVector(mBufSize , sizeof(OM_SUPPORT_TYPE), mHostBuffer, 1, mDvceBuffer, 1);
}

  void Atom :: setDims(int dim, int *dims)
  {
    mDim = dim;
    mDLen.clear();
    mBufSize=  1;
    for (int i = 0; i < dim; i++)
      {
        mDLen.push_back(dims[i]);
        mBufSize *= dims[i];
      }
  mInitialized = false; 
  mHostBuffer = (OM_SUPPORT_TYPE *)calloc(mBufSize, sizeof(OM_SUPPORT_TYPE));
  cublasStatus status = cublasAlloc(mBufSize, sizeof(OM_SUPPORT_TYPE), (void **)&mDvceBuffer);
  assert(status == CUBLAS_STATUS_SUCCESS);
  }

void Atom :: setScalaValue(OM_SUPPORT_TYPE v){
  mInitialized = true;
  *mHostBuffer = v;
  cublasSetVector(1, sizeof(OM_SUPPORT_TYPE), mHostBuffer, 1, mDvceBuffer, 1);
}


OM_SUPPORT_TYPE Atom :: getScalaValue(){
  return *mHostBuffer;
}


Atom :: ~Atom(){
  if(mHostBuffer)
    free(mHostBuffer);
  if(mDvceBuffer)
    cublasFree(mDvceBuffer);
    
}

OM_SUPPORT_TYPE Atom :: getElementAt(int x, int y) const{
  return mHostBuffer[y * mDLen[1] + x ];
}

void Atom :: setElementAt(int x, int y, OM_SUPPORT_TYPE ele){
  mHostBuffer[y * mDLen[1] + x] = ele;
}


void Atom :: setDim(int _dim, ...){
  int length = 1;

  mDim = _dim;
 

  mDLen.clear();

  va_list ap;
  va_start(ap, _dim);
  for (int i = 0; i < _dim; i++)
    mDLen.push_back(va_arg(ap, int));

  va_end(ap); 
  
  for(vector<int>::iterator it=mDLen.begin(); it!=mDLen.end(); it++){
    length *= *it;
  } 

  mHostBuffer = (OM_SUPPORT_TYPE *)realloc(mHostBuffer, length*sizeof(OM_SUPPORT_TYPE));
 
  mBufSize = length;

}

void Atom::syncFromDevice() 
{  
  cublasStatus status = cublasGetVector(getBufferSize(), sizeof(OM_SUPPORT_TYPE), mDvceBuffer, 1, mHostBuffer, 1);
  assert(status == CUBLAS_STATUS_SUCCESS);
}


void Atom::syncToDevice(){
	cublasStatus status = cublasSetVector(getBufferSize(), sizeof(OM_SUPPORT_TYPE), mHostBuffer, 1, mDvceBuffer, 1);
	assert(status == CUBLAS_STATUS_SUCCESS);
}

void Atom::streamAtom(stringstream &out) 
{
  if (getDim() == 1)
    {
      for (int i = 0; i != mDLen[0]; i++)
        out << mHostBuffer[i] << " " ;

      out << "<br>" << endl;
       //       for (int i = 0; 
    }
  else if (getDim() == 2)
    {
      for (int i = 0; i != mDLen[0]; i++)
        {
          for (int j = 0; j != mDLen[1]; j++)
            out << getElementAt(i,j) << " ";
          out << "<br>" <<  endl;
        }
      out << "<br>" << endl;
    }
  else
    {
      assert(0); // TODO not supported now
    }
}

} // namespace
