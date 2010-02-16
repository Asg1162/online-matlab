#pragma once

#include "common.h"
#include "../include/Matrix.h"
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

    Matrix *getX() const { return x;}
    Matrix *getY() const { return y;}
    /*    void dump(ifstream &))
    {
    }  */
    const char *writeToFile();
    int getNbCol() const;
    std::string getFileName() const { return mFileName; }

    void removeFile();
 private:
  Matrix *x;
  Matrix *y;
  char *mFormat;
  std::string mFileName;
};

} // namespace
