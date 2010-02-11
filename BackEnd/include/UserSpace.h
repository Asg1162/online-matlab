#pragma once

#include "common.h"

namespace ONLINE_MATLAB{


class Matrix;

/* each user has its own name space. 
 All variables' name are kept here */
 class Matlab;
class UserSpace{

public:
  explicit UserSpace(Matlab *parent);
  ~UserSpace();

  void newVar(const char *var);
  Matrix *getVar(const char *varName);
  int getSize();
  // bool returns whether the variable exists before or not
  bool updateVar(const char* varName, Matrix *newVal);

  // load matrix(es) from file
  Matrix *loadFrom(const char *filename, ...); 

  void saveMatrixTo(const char *filename, ...);

  Matrix *runFunction(char *funcName, int nooutput, int noargs, Matrix **matrix);
  const int getNumInstances() const 
  {
    return mNumInstances;
  }
  void incInstances()
  {
    mNumInstances++;
  }

  void decInstances()
  {
    mNumInstances--;
  }

 private:
  std::map<std::string, Matrix *> mVars; 
  typedef   std::map<std::string, Matrix *>::iterator varIter;
  Matlab *mParent;
  int mNumInstances;
};
 
} // namespace
