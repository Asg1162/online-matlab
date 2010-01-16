#pragma once

#include "common.h"
#include "assert.h"
#include <sstream>
/* This base class encapsulates gpu memory manipulations
 */

namespace ONLINE_MATLAB {
class Atom {
public:
  explicit  Atom(int dim);
  Atom(int dim, int *dims);
  Atom(int dim, int *dims, OM_SUPPORT_TYPE *elements);
  ~Atom();

  void setDim(int dim, ...);
  void setDims(int dim, int *);
  //int getDim();
  int getDim() const { return mDim;}
  int getDimAt(int i) const { assert(i <= mDim);
    return mDLen[i];
} 
  int getLength(){
      return mDLen.size();
  }

  void setScalaValue(OM_SUPPORT_TYPE v);
  OM_SUPPORT_TYPE getScalaValue();
    bool initialized() const 
    { return mInitialized;}
    void setInitialized(bool b)
    { mInitialized = b; }

    OM_SUPPORT_TYPE getElementAt(int x, int y) const;
    void setElementAt(int x, int y, OM_SUPPORT_TYPE ele);

    OM_SUPPORT_TYPE *getInternalBuffer() { return mHostBuffer; }

 protected:
    void streamAtom(std::stringstream &out); 
private:

    //    void initGpu();
  OM_SUPPORT_TYPE *mHostBuffer;
  void *mDvceBuffer;
  bool mInitialized;
  int mDim;
  std::vector<int> mDLen;

  int mBufSize;

};

}//namespace 
