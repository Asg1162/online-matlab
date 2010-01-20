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
 Matrix *omSgerand(int nooutput, int noargs, Matrix **);


 Matrix *omSgesvd(int nooutput, int noargs, Matrix **);

 Matrix *omPlot(int nooutput, int noargs, Matrix **matrices);

    // Matrix *omDgerand(int nooutput, int noargs, Matrix **);

}