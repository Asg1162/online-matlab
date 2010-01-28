#include "UserSpace.h"
#include "Matrix.h"
#include "FunctionList.h"
#include "Matlab.h"
#include "ExeException.h"

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
     if (func != 0) // normal function
       result = func(nooutputs, noargs, matrix);
     else // it's not a function, it's a submatrix of a variable
       {
         if(mVars.find(funcName) == mVars.end())
           {
             string error(funcName);
             error += " not found";
             throw ExeException(error);
           }
         //
         if (noargs == 1)
           {
             Matrix *index = matrix[0];

             result = new Matrix(NULL, index->getDim(), index->getDimAt(0), index->getDimAt(1));
             Matrix *var = getVar(funcName);
             OM_SUPPORT_TYPE *buffer = var->getInternalBuffer();
             for (int i = 0; i != index->getDimAt(0); i++)
               for (int j = 0; j != index->getDimAt(1); j++)
                 {
                   int offset = index->getElementAt(i, j) - 1;
                   if (offset >= var->getBufferSize())
                     {
                       delete result;
                       throw ExeException("Index exceeds matrix dimension");
                     }
                   result->setElementAt(i, j, buffer[offset]);
                 }
           }
         else if (noargs == 2)
           {
             int dim1 = matrix[0]->getBufferSize();
             int dim2 = matrix[1]->getBufferSize();
             result = new Matrix(NULL, 2, dim1, dim2);
             int curRow(0);
             Matrix *var = getVar(funcName);
             int curColumn(0);
             for (int j = 0; j != matrix[0]->getDimAt(1); j++)
               {
                 for (int i = 0; i != matrix[0]->getDimAt(0); i++)
                   {
                     curColumn = 0;
                     for (int y = 0; y != matrix[1]->getDimAt(1); y++)
                       {
                         for (int x = 0; x != matrix[1]->getDimAt(0); x++)
                           {
                             int idxx = matrix[0]->getElementAt(i, j) - 1;
                             int idxy = matrix[1]->getElementAt(x, y) - 1;
                             if (idxx >= var->getDimAt(0) || idxy >= var->getDimAt(1))
                               {
                                 delete result;
                                 throw ExeException("Index exceeds matrix dimension");
                               }
                             result->setElementAt(curRow, curColumn, var->getElementAt(idxx, idxy));
                             curColumn++;
                           }
                       }
                     curRow++;
                   }
               }
           }
         result->syncToDevice();
         result->setInitialized(true);
       }
       

     return result;
   }

}
