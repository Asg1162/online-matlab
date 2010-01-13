#include "Matlab.h"
#include <cassert>
#include "FunctionList.h"
#include "Gpu.h"
#include <iostream>

namespace ONLINE_MATLAB{

  using namespace std;

Matlab :: Matlab(int gpuId){
  // build the function map
  mFunctions["rand"] = omSgerand;
  mFunctions["svd"] = omSgesvd;
  int r = initGpu(gpuId);
  if (r != 0)
    {
      std::cout << "failed to initialize GPU, quitting...\n" << std::endl;
      exit(0);
    }
}

Matlab :: ~Matlab(){
  std::map<std::string, UserSpace*>::iterator it;
  for(it=mUsers.begin(); it!=mUsers.end(); it++){
    delete it->second;
  }
  mUsers.clear();

}

void Matlab :: newUser(std::string name){
  // TODO more robust
  assert(mUsers.find(name) == mUsers.end());
  mUsers[name] = new UserSpace(this); 
}

void Matlab :: delUser(std::string name){
  delete(mUsers[name]);
  mUsers.erase(name);
}

UserSpace* Matlab :: getUser(std::string name){
  return mUsers[name];
}

 OM_FUNCTION Matlab :: getFunction(const std::string &funcName) 
   {
     // TODO throw exception
     assert(mFunctions.find(funcName) != mFunctions.end());

     return mFunctions[funcName];
   }

}
