#pragma once

#include "common.h"
#include "Atom.h"
namespace ONLINE_MATLAB {

class Matrix : public Atom {
  
  public:
    //    Matrix();
    Matrix(int dim, ...);
  Matrix(int dim, int *dims, OM_SUPPORT_TYPE *elements);
  //    Matrix();
    ~Matrix();


    void allocMatrix();

    Matrix *operator*(Matrix const &rhs) const;

    Matrix *getNext() const
    {
      return mNext;
    }

    void setNext(Matrix *next)
    {
      mNext = next;
    }

  private:
    Matrix *mNext;  // currently used when the function has multiple returns
};


}// namespace 
