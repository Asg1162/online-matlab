#include "UserSpace.h"
#include "Matrix.h"
#include "FunctionList.h"
#include "Matlab.h"

namespace ONLINE_MATLAB{


  UserSpace :: UserSpace(Matlab *parent):
  mParent(parent),
    mNumInstances(1)
{}

UserSpace :: ~UserSpace(){
  std::map<std::string, Matrix *>::iterator it;
  for(it = mVars.begin(); it!=mVars.end(); it++){
    delete it->second;
  }
  mVars.clear();
}

 void UserSpace :: newVar(const char* varName){
   //   if (mVars[varName]  == 0)
   //    {
   //      Atom *a = new Matrix();
   std::string v(varName);
   mVars.insert(std::pair<std::string, Matrix *>(v, 0));
}

 void UserSpace :: updateVar(const char* varName, Matrix *newVal){
   std::string v(varName);
   if (mVars.find(v) != mVars.end()) // if found
     {
       if (mVars[v] != 0)
         delete mVars[v];
       mVars[v] = newVal;
     }
   else
     {
       mVars.insert(std::pair<std::string, Matrix *>(v, newVal));
     }
   varIter it = mVars.find(v);
   newVal->updateName((*it).first.c_str());
 }


Matrix* UserSpace :: getVar(const char *varName){
  std::string v(varName);

  if(mVars.find(v) == mVars.end())
    printf("trying to get %s failed.\n", varName);
  assert(mVars.find(v) != mVars.end());

  return mVars[v];
 }

int UserSpace :: getSize(){
  return mVars.size();
}


 Matrix *UserSpace :: runFunction(char *funcName, int nooutputs, int noargs, Matrix **matrix)
   {
     Matrix *result;
     std::string fun(funcName);
     OM_FUNCTION func = mParent->getFunction(fun);
     result = func(nooutputs, noargs, matrix);

     return result;
   }

}
