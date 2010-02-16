#include "../include/common.h"
#include "../include/FunctionList.h"
#include "../include/Matlab.h"
#include <cstdlib>
#include "../include/Gpu.h"
#include "../include/ExeException.h"

#include "../../3rdparty/pngwriter-0.5.4/src/pngwriter.h"
#include <time.h>
#include <sstream>
#include <string>




using namespace std;

extern ONLINE_MATLAB::Matlab *gMatlab;
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
  int dims[noargs+1];
  int buffersize=1;
  if (noargs == 1)
    {
      dims[0] = 1;
      dims[1] = (int)matrices[0]->getScalaValue();
      buffersize = dims[1];
      noargs++; // make it to two dimension
    }
  else
    {
      for (int i = 0; i != noargs; i++)
        {
          dims[i] = (int)matrices[i]->getScalaValue();
          buffersize *= dims[i];
        }
    }

  OM_SUPPORT_TYPE *elements = new OM_SUPPORT_TYPE[buffersize];
  for (int i = 0; i != buffersize; i++)
    {
      elements[i] = rand()/(OM_SUPPORT_TYPE)RAND_MAX;
    }

  Matrix *re  = new Matrix(NULL, noargs, dims, elements);
  
  delete [] elements;
  re->setNext(0);

  re->syncToDevice();
  return re;
}


Matrix *omSgeeig(int nooutput, int noargs, Matrix **matrices){
	if (noargs != 1)
		throw ExeException("Eig accepts one matrix.");
	if (nooutput != 1 && nooutput != 2 && nooutput != 3)
		throw ExeException("Eig accepts 1, 2, or 3 output matrices.");

	int n = matrices[0]->getDimAt(0);
	if(n != matrices[0]->getDimAt(1) )
		throw ExeException("Input to eig should be a square matrix.");

	Matrix *WR, *WI, *VL, *VR;
	WR = new Matrix(NULL, 2, n, 1);
	WI = new Matrix(NULL, 2, n, 1);

	char jobvl, jobvr;
	culaStatus status;
	switch(nooutput){
		case 1:
				jobvl = 'N';
				jobvr = 'N';
				status = culaSgeev(jobvl, jobvr, n, matrices[0]->getInternalBuffer(), n, WR->getInternalBuffer(), WI->getInternalBuffer(), NULL, n, NULL, n);
				checkStatus(status);
				break;
		case 2:
				jobvl = 'V';
				jobvr = 'N';
				VL = new Matrix(NULL, 2, n, n);
				status = culaSgeev(jobvl, jobvr, n, matrices[0]->getInternalBuffer(), n, WR->getInternalBuffer(), WI->getInternalBuffer(), VL->getInternalBuffer(), n, NULL, n);
				checkStatus(status);
				VL->syncToDevice();
				VL->setInitialized(true);
				WR->setNext(VL);
				break;
		case 3:
				jobvl = 'V';
				jobvr = 'V';
				VL = new Matrix(NULL, 2, n, n);
				VR = new Matrix(NULL, 2, n, n);
				status = culaSgeev(jobvl, jobvr, n, matrices[0]->getInternalBuffer(), n, WR->getInternalBuffer(), WI->getInternalBuffer(), VL->getInternalBuffer(), n, VR->getInternalBuffer(), n);
				checkStatus(status);
				VL->syncToDevice();
				VR->syncToDevice();
				VL->setInitialized(true);
				VR->setInitialized(true);
				WR->setNext(VL);
				VL->setNext(VR);
				break;
	}

	WR->syncToDevice();
	WR->setInitialized(true);
	//WI->syncToDevice();
	delete WI;

	return WR;

}

 Matrix *omSgelu(int nooutput, int noargs, Matrix **matrices)
 {
    if (noargs != 1)
      throw ExeException("lu supports one matrix.");

   if (nooutput > 1)
      throw ExeException("lu currently supports one  outputs.");

    assert(matrices[0]->getDim() == 2);

    int M = matrices[0]->getDimAt(0);
    int N = matrices[0]->getDimAt(1);

    Matrix *output = matrices[0]->clone();

    int lda = M;

    printf("Calling culaSgelu\n");
    // TODO make ipiv a matrix
	int ipiv[min(M,N)];
	for(int i=1; i<=min(M,N); i++){
		ipiv[i-1] = i;
	}
    culaStatus status = culaSgetrf(M, N, output->getInternalBuffer(), M,ipiv);
    checkStatus(status);

    if (nooutput == 1)
      {
		output->syncToDevice();
        output->setInitialized(true);
        return output;
      }
    /*
    else{
      U->setNext(S);
	  U->syncToDevice();
      U->setInitialized(true);
      S->setNext(VT);
	  S->syncToDevice();
      S->setInitialized(true);
      VT->setNext(NULL);
	  VT->syncToDevice();
      VT->setInitialized(true);      
      return U;
      }*/
   
 }

Matrix *omSgeinv(int nooutput, int noargs, Matrix **matrices){

    if (noargs != 1)
      throw ExeException("Inv supports one matrix.");

	int n = matrices[0]->getDimAt(0);
	if(n != matrices[0]->getDimAt(1) )
      throw ExeException("Input to inv should be a square matrix.");

	Matrix *A = matrices[0]->clone();

	int ipiv[n];
	for(int i=1; i<=n; i++){
		ipiv[i-1] = i;
	}

	culaStatus status = culaSgetri(n, A->getInternalBuffer(), n, ipiv);

	checkStatus(status);

	A->syncToDevice();
    A->setInitialized(true);
			
	return A;
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
		S->syncToDevice();
        S->setInitialized(true);
        return S;
      }
    else{
      U->setNext(S);
	  U->syncToDevice();
      U->setInitialized(true);
      S->setNext(VT);
	  S->syncToDevice();
      S->setInitialized(true);
      VT->setNext(NULL);
	  VT->syncToDevice();
      VT->setInitialized(true);      
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
    m->setInitialized(true);
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
    mm->setInitialized(true);
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
        m->setInitialized(true);
		return m;

}

// declaration, forward
 extern "C" void omgSin(int maxthread,const float *bufferin, float *bufferout);
 extern "C" void omgCos(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgTan(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgCot(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgLog(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgLog2(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgLog10(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgExp(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgAbs(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgFix(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgCeil(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgFloor(int numElement, const float *bufferin, float *bufferout);
 extern "C" void omgRound(int numElement, const float *bufferin, float *bufferout);

Matrix *omSin(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("sin accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

    omgSin(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
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
    m->setInitialized(true);
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
    m->setInitialized(true);
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
    m->setInitialized(true);
	return m;
}

Matrix *omLog(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("Hilb accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgLog(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
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
    m->setInitialized(true);
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
    m->setInitialized(true);
	return m;
}

Matrix *omExp(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("exp accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgExp(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}
Matrix *omAbs(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("abs accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgAbs(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}

Matrix *omRound(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("round accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgRound(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}
Matrix *omFix(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("fix accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgFix(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));    
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}
Matrix *omCeil(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("ceil accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgCeil(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}
Matrix *omFloor(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1)
			throw ExeException("floor accepts 1 argument.\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
	Matrix *m = new Matrix(NULL, 2, dim1, dim2);

	// code goes here
    omgFloor(matrices[0]->getBufferSize(), matrices[0]->getDevicePtr(), const_cast<float *>(m->getDevicePtr()));
	m->syncFromDevice();
    m->setInitialized(true);
	return m;
}


// CPU version
Matrix *omSum(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1 && noargs != 2)
			throw ExeException("sum accepts 1 or 2 argument(s).\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);

    Matrix *m;
	// code goes here
    if (dim1 == 1 || dim2 == 1) // if it is a vector
      {
        m = new Matrix(NULL, 2, 1, 1);  //  the result is a vector
        OM_SUPPORT_TYPE total = 0.0;
        OM_SUPPORT_TYPE *element = matrices[0]->getInternalBuffer();
        for (int i = 0; i != matrices[0]->getBufferSize(); i++)
          total += *element++;
        m->setScalaValue(total);
      }
    else
      {
        OM_SUPPORT_TYPE total[dim2];
        for (int i = 0; i != dim2; i++)
          {
            total[i] = 0.0;
            for (int j = 0; j != dim1; j++)
              {
                total[i] += matrices[0]->getElementAt(j,i);
              }
          }
        int dims[2]; dims[0] = 1; dims[1] = dim2;
        m = new Matrix(NULL, 2, dims, total);
      }

	m->syncToDevice();
    m->setInitialized(true);
	return m;
}

Matrix *omProd(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1 && noargs !=2 )
			throw ExeException("prod accepts 1 or 2 argument(s).\n");
	int dim1 = matrices[0]->getDimAt(0);
	int dim2 = matrices[0]->getDimAt(1);
    Matrix *m;

	// code goes here
    if (noargs == 1) // only one argument is provided:
      {
        if (dim1 == 1 || dim2 == 1) // if it is a vector
          {
            m = new Matrix(NULL, 2, 1, 1);  //  the result is a vector
            OM_SUPPORT_TYPE total = 1.0;
            OM_SUPPORT_TYPE *element = matrices[0]->getInternalBuffer();
            for (int i = 0; i != matrices[0]->getBufferSize(); i++)
              total *= *element++;
            m->setScalaValue(total);
          }
        else
          {
            OM_SUPPORT_TYPE total[dim2];
            for (int i = 0; i != dim2; i++)
              {
                total[i] = 1.0;
                for (int j = 0; j != dim1; j++)
                  {
                    total[i] *= matrices[0]->getElementAt(j,i);
                  }
              }
            int dims[2]; dims[0] = 1; dims[1] = dim2;
            m = new Matrix(NULL, 2, dims, total);
          }
      }
    else  // two argument 
      {
        int DIM = matrices[1]->getScalaValue();
        if (DIM == 2) // row-wise product
          {
            m = new Matrix(NULL, 2, dim1, 1);
            for (int i = 0; i != dim1; i++)
              {
                OM_SUPPORT_TYPE total(1.0);
                for (int j = 0; j != dim2; j++)
                  total *= matrices[0]->getElementAt(i, j);
                m->setElementAt(i, 0, total);
              }
          }
        else
         {
            m = new Matrix(NULL, 2, 1, dim2);
            for (int i = 0; i != dim2; i++)
              {
                OM_SUPPORT_TYPE total(1.0);
                for (int j = 0; j != dim1; j++)
                  total *= matrices[0]->getElementAt(j, i);
                m->setElementAt(0, i, total);
              }
         }
      }

	m->syncToDevice();
    m->setInitialized(true);
	return m;

}


Matrix *omSMin(int nooutput, int noargs, Matrix **matrices){
	if(noargs != 1 && noargs != 2)
      throw ExeException("min accepts 1 or 2 argument(s).\n");
    
    Matrix *m;
    if (noargs == 1)
      {
        int dim1 = matrices[0]->getDimAt(0);
        int dim2 = matrices[0]->getDimAt(1);
        Matrix *m;
        
        // code goes here
        if (dim1 == 1 || dim2 == 1) // if it is a vector
          {
            m = new Matrix(NULL, 2, 1, 1);  //  the result is a vector
            OM_SUPPORT_TYPE total = 1.0;
            OM_SUPPORT_TYPE *element = matrices[0]->getInternalBuffer();
            for (int i = 0; i != matrices[0]->getBufferSize(); i++)
              total *= *element++;
            m->setScalaValue(total);
          }
        else
          {
            OM_SUPPORT_TYPE total[dim2];
            for (int i = 0; i != dim2; i++)
              {
                total[i] = 1.0;
                for (int j = 0; j != dim1; j++)
                  {
                    total[i] *= matrices[0]->getElementAt(j,i);
                  }
              }
            int dims[2]; dims[0] = 1; dims[1] = dim2;
            const OM_SUPPORT_TYPE *t = total;
            m = new Matrix(NULL, 2, dims, t);
          }
        
        m->syncToDevice();
        m->setInitialized(true);
      }
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
        m->setInitialized(true);
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
        m->setInitialized(true);
		return m;

}

 extern char filePath[];

Matrix *omLoad(int nooutput, int noargs, Matrix **matrices){
  //		if(noargs != 1 && noargs != 2)
  //			throw ExeException("Hilb accepts 1 or 2 argument(s).\n");

  char *orig = (char *)matrices[0];
  string filename = string(&orig[1]); //get rid of the first "

  string final = filename.substr(0, filename.size()-1);   

  stringstream matrixFile;
  matrixFile << filePath << gCurUser << "/" << final ;

  gMatlab->getUser(gCurUser)->loadFrom(matrixFile.str().c_str());
  return 0;
}













} // namespace 
