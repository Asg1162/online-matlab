#include "../include/common.h"
#include "../include/FunctionList.h"
#include <cstdlib>
#include "../include/Gpu.h"

namespace ONLINE_MATLAB {

  Matrix *omSgerand(int nooutput, int noargs, Matrix **matrices)
{
  assert(nooutput == 1);
  int dims[noargs];
  int buffersize=1;
  for (int i = 0; i != noargs; i++)
    {
      dims[i] = (int)matrices[i]->getScalaValue();
      buffersize *= dims[i];
    }

  OM_SUPPORT_TYPE elements[buffersize];
  for (int i = 0; i != buffersize; i++)
    {
      elements[i] = rand()/(OM_SUPPORT_TYPE)RAND_MAX;
    }

  Matrix *re  = new Matrix(NULL, noargs, dims, elements);
    
  re->setNext(0);
  return re;
}

  Matrix *omSgesvd(int nooutput, int noargs, Matrix **matrices)
  {
    assert(nooutput == 1 || nooutput == 3);

    assert(noargs == 1);
    assert(matrices[0]->getDim() == 2);
    int M = matrices[0]->getDimAt(0);
    int N = matrices[0]->getDimAt(1);
    Matrix *U = new Matrix((const char *)NULL, 2, M, M);
    int ucol = M<N? M: N;
    Matrix *S = new Matrix(NULL, 2, 1, ucol);
    Matrix *VT = new Matrix(NULL, 2, N, N);
    printf("doing svd for matric %d X %d", M, N);

    int lda = M;
    int ldu = N;
    int ldvt = N;

    printf("Calling culaSgesv\n");
    culaStatus status = culaSgesvd('A', 'A', M, N, matrices[0]->getInternalBuffer(), lda, S->getInternalBuffer(), U->getInternalBuffer(), ldu, VT->getInternalBuffer(), ldvt);
    checkStatus(status);
    /*    S = (float*)malloc(imin(M,N)*sizeof(float));
    U = (float*)malloc(LDU*M*sizeof(float));
    VT = (float*)malloc(LDVT*N*sizeof(float));
    */
    if (nooutput == 1)
      {
        delete U;  delete VT;
        return S;
      }
    else{
      U->setNext(S);
      S->setNext(VT);
      VT->setNext(NULL);
      return U;
    }
  }
} // namespace 
