#pragma once

#include "../include/Matrix.h"

namespace ONLINE_MATLAB {

  typedef   Matrix*(*OM_FUNCTION)(int, int, Matrix **);

  /* Name conventions 
     om          S         ge               svd
    ---      --------   -----------      ------------
    prefix  data type   matrix type       routine name

    Currently(will) support:
    data type: S(float)  D(double
    matrix type: ge (general), bd(bidiagonal), or(orthogonal real) tr(triangular) un(complex unary)
    routine name: trf, sv, qrf, svd, ls
   */
 Matrix *omHilb(int nooutput, int noargs, Matrix **);

 Matrix *omMagic(int nooutput, int noargs, Matrix **);
 Matrix *odd_order(int n);
 Matrix *doubly_even(int n);
 Matrix *singly_even(int n);

 Matrix *omEye(int nooutput, int noargs, Matrix **);

 Matrix *omOnes(int nooutput, int noargs, Matrix **);

 Matrix *omZeros(int nooutput, int noargs, Matrix **);
 Matrix *omSin(int nooutput, int noargs, Matrix **);
 Matrix *omCos(int nooutput, int noargs, Matrix **);
 Matrix *omTan(int nooutput, int noargs, Matrix **);
 Matrix *omCot(int nooutput, int noargs, Matrix **);
 Matrix *omLog(int nooutput, int noargs, Matrix **);
 Matrix *omLog2(int nooutput, int noargs, Matrix **);
 Matrix *omLog10(int nooutput, int noargs, Matrix **);
 Matrix *omExp(int nooutput, int noargs, Matrix **);
 Matrix *omAbs(int nooutput, int noargs, Matrix **);
 Matrix *omRound(int nooutput, int noargs, Matrix **);
 Matrix *omFix(int nooutput, int noargs, Matrix **);
 Matrix *omCeil(int nooutput, int noargs, Matrix **);
 Matrix *omFloor(int nooutput, int noargs, Matrix **);
 Matrix *omSum(int nooutput, int noargs, Matrix **);
 Matrix *omProd(int nooutput, int noargs, Matrix **);

 Matrix *omSgerand(int nooutput, int noargs, Matrix **);

 Matrix *omSgesvd(int nooutput, int noargs, Matrix **);

 Matrix *omPlot(int nooutput, int noargs, Matrix **matrices);

    // Matrix *omDgerand(int nooutput, int noargs, Matrix **);

}
