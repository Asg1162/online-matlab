#include "../include/common.h"
#include "../include/FunctionList.h"
#include <cstdlib>
#include "../include/Gpu.h"

namespace ONLINE_MATLAB {

  Matrix *omSgerand(int nooutput, int noargs, Matrix **matrices)
{
  assert(nooutput == 1);
  printf("init rand dimension (%d %d).\n", (int)matrices[0]->getScalaValue(), (int)matrices[1]->getScalaValue());
  Matrix *re  = new Matrix(noargs, (int)matrices[0]->getScalaValue(), (int)matrices[1]->getScalaValue());  
  printf("get dim = %d. \n", re->getDim());
  assert(re->getDim() == 2);
  re->setInitialized(true);
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
    Matrix *U = new Matrix(2, M, M);
    int ucol = M<N? M: N;
    Matrix *S = new Matrix(2, 1, ucol);
    Matrix *VT = new Matrix(2, N, N);
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
      return U;
    }
  }
} // namespace 
