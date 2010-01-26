#pragma once

#include "common.h"
#include "Atom.h"
namespace ONLINE_MATLAB {

class Matrix : public Atom {
  
  public:
    //    Matrix();
    Matrix(const char *name, int dim, ...);
    Matrix(const char *name, int dim, int *dims, OM_SUPPORT_TYPE *elements);


  //    Matrix();
    ~Matrix();


    void allocMatrix();

    Matrix *operator+(Matrix const &rhs) const;
    Matrix *operator-(Matrix const &rhs) const;
    Matrix *operator*(Matrix const &rhs) const;

    Matrix *transpose();

    Matrix *getNext() const
    {
      return mNext;
    }
    const char *getName() const
    {
      return mName;
    }

    void updateName(const char *name)
    {
      mName = name;
    }
    void setNext(Matrix *next)
    {
      mNext = next;
    }

    void streamOut(std::stringstream &out);

  private:
    Matrix(const Matrix &rhs); // copy constructor not supported yet

    Matrix *mNext;  // currently used when the function has multiple returns
    const char *mName;
};


}// namespace 
