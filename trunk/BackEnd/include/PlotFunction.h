#pragma once

#include "common.h"
namespace ONLINE_MATLAB{
  using namespace std;
  

class PlotPair {
 public:
  explicit PlotFunction(int noargs)
    :mNoArgs(noargs)
  {
    mMatrixArray = new (Matrix*)[noargs];
  }


 private:
  int mNoArgs;
  Matrix **mMatrixArray;

};

} // namespace
