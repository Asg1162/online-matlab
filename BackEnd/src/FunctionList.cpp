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
    fns << gCurUser << "/" << gCurUser << "_" << seconds << ".png" << endl; 
    char *filename = new char[strlen(fns.str().c_str())];
    strcpy(filename, fns.str().c_str());
    return (Matrix *)filename;
  }


Matrix *omMagic( int nooutput, int noargs, Matrix **matrices ){
    // generate a magic square using the appropriate method
    if(noargs != 1)
    	throw ExeException("Hilb accepts 1 argument.\n");
    int dim;
    dim = (int) matrices[0]->getScalaValue();
    if(dim < 1 || dim == 2){
    	throw ExeException("Dimension should be a positive integer other than 2.\n");
    }

    if (dim%2 > 0)
		return odd_order(dim);
    else if (dim%4 == 0)  
        return doubly_even(dim);
	else
		return singly_even(dim);
}

Matrix *odd_order( int n ){
    //generate a magic square using the
    //siamese method for any odd value n

    int i, j, N3, t;
    Matrix *m = new Matrix(NULL, 2, n, n);

    N3 = (n - 3) / 2;
    
    for (i=1; i<=n; i++ ){
    	for (j=1; j<=n; j++){
			t = (i + j*2 - 2) % n;
			m->setElementAt(i-1, j-1, (1 + t + n*((N3 + i + j)%n)) );
    	}
    }

    m->syncToDevice();
    return m;
}

Matrix *doubly_even( int n ){
    //generate a magic square using the
    //big X method for sizes 4, 8, 12, 16 ...

    int i, j;
    int nsq1 = n*n + 1;
    int val  = 0;
    Matrix *m = new Matrix(NULL, 2, n, n);

    for (i=1; i<=n; i++ ){
    	for (j=1; j<=n; j++){
			val++; 
			if ( ( ((i/2)%2 > 0) && ((j/2)%2 > 0) ) || ( ((i/2)%2 == 0) && ((j/2)%2 == 0)) )
				m->setElementAt(i-1,j-1, nsq1 - val);
			else
				m->setElementAt(i-1, j-1, val);
      	}
    }

    m->syncToDevice();
    return m;
}

Matrix *singly_even( int n ){
    //generate a magic square using the 
    //LUX method for sizes  6, 10, 14 ...

    int i, j, s3, t, s, m;
    int js=0, is=0;
    Matrix *mm = new Matrix(NULL, 2, n, n);
   
    m = (n - 2) / 4;
    s = (m*2) + 1;
    s3 = (s - 3) / 2;
    
    for( i=1; i<=n; i++){

		if ((i%2)>0) is++;

		js = 0;

		for (j=1; j<=n; j++){
			if ((j%2)>0) js++;

			t = ((((is + (js*2) - 2)%s) + s*((s3 + is + js)%s))*4) + 1;

			if ((i % 2) > 0){ 
				if ( (is > m+2) || ( (is == m+2) && (js != m+1) ) || ( (is == m+1) && (js  == m+1) ) ){
	    			if (j % 2 > 0) {
						mm->setElementAt(i-1, j-1, t); 
	    			} else {
	      				mm->setElementAt(i-1, j-1, t+3); 
	    			}
	  			} else {
	    			if (j % 2 > 0) {
	      				mm->setElementAt(i-1, j-1, t+3); 
	    			} else {
	      				mm->setElementAt(i-1, j-1, t); 
	    			}
	  			}
			} else {                                            
	  			if (is < m+3){               
	    			if (j % 2 > 0)
	      				mm->setElementAt(i-1, j-1, t+1); 
	    			else
	      				mm->setElementAt(i-1, j-1, t+2); 
	  			} else {
	    			if (j % 2 > 0)
	      				mm->setElementAt(i-1, j-1, t+2); 
	    			else
	      				mm->setElementAt(i-1, j-1, t+1);          
	  			}
			}
		}
	}
    mm->syncToDevice();
    return mm;
}


Matrix *omEye(int nooutput, int noargs, Matrix **matrices){
		if(noargs != 1 && noargs != 2)
			throw ExeException("Hilb accepts 1 or 2 argument(s).\n");
		int dim1, dim2;
		dim1 = (int) matrices[0]->getScalaValue();
		if(noargs == 2)
			dim2 = (int) matrices[1]->getScalaValue();
		else
			dim2 = dim1;

		if(dim1 < 1 || dim2 < 1){
			throw ExeException("Dimension should be a positive integer.\n");
		}

		Matrix *m = new Matrix(NULL, 2, dim1, dim2);

		for(int i=0; i<dim1 && i<dim2; i++){
			m->setElementAt(i, i, 1);
		}

		m->syncToDevice();
		return m;

}

// declaration, forward
 extern "C" void omgSin(int maxthread,const float *bufferin, float *bufferout);
 extern "C" void omgCos(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgTan(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgCot(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgLog2(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgLog10(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgExp(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgAbs(int numElement, const float *bufferin, float *bufferout);

 extern "C" void omgCeil(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgFloor(int numElement, const float *bufferin, float *bufferout);

Matrix *omSin(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("sin accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

    omgSin(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}

Matrix *omCos(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
      throw ExeException("cos accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgCos(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}

Matrix *omTan(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("tan accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgTan(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}
Matrix *omCot(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("cot accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgCot(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}

Matrix *omLog(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here

	m->syncFromDevice();
	return m;
}

Matrix *omLog2(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("log2 accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgLog2(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}
Matrix *omLog10(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgLog10(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}

Matrix *omExp(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgExp(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}
Matrix *omAbs(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgAbs(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}
Matrix *omRound(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here

	m->syncFromDevice();
	return m;
}
Matrix *omFix(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here

	m->syncFromDevice();
	return m;
}
Matrix *omCeil(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgCeil(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}
Matrix *omFloor(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgFloor(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
	return m;
}

Matrix *omSum(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here

	m->syncFromDevice();
	return m;
}
Matrix *omProd(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here

	m->syncFromDevice();
	return m;
}















Matrix *omOnes(int nooutput, int noargs, Matrix **matrices){
		if(noargs != 1 && noargs != 2)
			throw ExeException("Hilb accepts 1 or 2 argument(s).\n");
		int dim1, dim2;
		dim1 = (int) matrices[0]->getScalaValue();
		if(noargs == 2)
			dim2 = (int) matrices[1]->getScalaValue();
		else
			dim2 = dim1;

		if(dim1 < 1 || dim2 < 1){
			throw ExeException("Dimension should be a positive integer.\n");
		}

		Matrix *m = new Matrix(NULL, 2, dim1, dim2);

		for(int i=0; i<dim1; i++){
			for(int j=0; j<dim2; j++){
				m->setElementAt(i, j, 1);
			}
		}

		m->syncToDevice();
		return m;

}

Matrix *omZeros(int nooutput, int noargs, Matrix **matrices){
		if(noargs != 1 && noargs != 2)
			throw ExeException("Hilb accepts 1 or 2 argument(s).\n");
		int dim1, dim2;
		dim1 = (int) matrices[0]->getScalaValue();
		if(noargs == 2)
			dim2 = (int) matrices[1]->getScalaValue();
		else
			dim2 = dim1;

		if(dim1 < 1 || dim2 < 1){
			throw ExeException("Dimension should be a positive integer.\n");
		}

		Matrix *m = new Matrix(NULL, 2, dim1, dim2);
		m->syncToDevice();
		return m;

}














} // namespace 
