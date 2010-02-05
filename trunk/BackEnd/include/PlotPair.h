#pragma once

#include "common.h"
namespace ONLINE_MATLAB{
  using namespace std;
  

class PlotPair {
 public:
 PlotPair(Matrix *xx, Matrix *yy, char *format):
    x(xx),
      y(yy),
    mFormat(format)
   {
   }

    /*    void dump(ifstream &))
    {
    }  */

 private:
  Matrix *x;
  Matrix *y;
  char *mFormat;

};

} // namespace
