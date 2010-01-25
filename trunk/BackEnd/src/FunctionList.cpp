#include "../include/common.h"
#include "../include/FunctionList.h"
#include <cstdlib>
#include "../include/Gpu.h"
#include "../include/ExeException.h"

#include "../../3rdparty/pngwriter-0.5.4/src/pngwriter.h"
#include <time.h>
#include <sstream>

using namespace std;
 extern std::string gCurUser;
namespace ONLINE_MATLAB {

  Matrix *omHilb(int nooutput, int noargs, Matrix **matrices){
	//assert(dim>=1);
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim;
	dim = (int) matrices[0]->getScalaValue();
	if(dim < 1){
			throw ExeException("Dimension should be a positive integer.\n");
	}
	Matrix *h = new Matrix((const char *)NULL, 2, dim,  dim);
	for(int i=0; i<dim; i++){
		for(int j=0; j<dim; j++){
			h->setElementAt(i, j, (OM_SUPPORT_TYPE)1/(OM_SUPPORT_TYPE)(i+j+1));
		}
	}

	h->syncToDevice();
	return h;
  }

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

  re->syncToDevice();
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
		S->syncFromDevice();
        return S;
      }
    else{
      U->setNext(S);
	  U->syncFromDevice();
      S->setNext(VT);
	  S->syncFromDevice();
      VT->setNext(NULL);
	  VT->syncFromDevice();
      return U;
    }
  }

  static char filePath[] = "/home/xwu3/matcloud/mysite/media/users/";

  Matrix *omPlot(int nooutput, int noargs, Matrix **matrices)
  {
    // generate name from current time
    time_t seconds;

    seconds = time (NULL);

    stringstream ss;
    ss << filePath << gCurUser << "/" << gCurUser << "_" << seconds << ".png" ;
    cout << "user file name is "<< ss.str() << endl;

    int picWidth = 200;
    int picLength = 300;
    pngwriter picture(picWidth,picLength, 65535, ss.str().c_str());

    if (noargs > 1)
      throw ExeException("plot only supports one matrix for now");

    if (noargs == 1)
      {
        // plot one matrix
        // TODO find the maxu
        cout << " write file." << endl;
        OM_SUPPORT_TYPE *element = matrices[0]->getInternalBuffer();
        int numElements = matrices[0]->getBufferSize();
        for (int i = 0; i != numElements; i++)
          picture.plot(i * picLength/(float)numElements, *element++, 1.0, 0.0 ,0.0);
      }

    picture.close();

    stringstream fns;
    fns << gCurUser << "/" << gCurUser << "_" << seconds << ".png"; 
    char *filename = new char[strlen(fns.str().c_str())];
    strcpy(filename, fns.str().c_str());
    return (Matrix *)filename;
  }

} // namespace 