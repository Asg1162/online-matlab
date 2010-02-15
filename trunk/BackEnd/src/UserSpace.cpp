#include "UserSpace.h"
#include "Matrix.h"
#include "FunctionList.h"
#include "Matlab.h"
#include "ExeException.h"
#include "LoadFun.h"
#include <fstream>
#include <string>
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

 bool UserSpace :: updateVar(const char* varName, Matrix *newVal){
   bool exists;
   std::string v(varName);
   if (mVars.find(v) != mVars.end()) // if found
     {
       exists = true;
       if (mVars[v] != 0)
         delete mVars[v];

       if (newVal->getName() == 0) // if the new value is anonymous
         mVars[v] = newVal;
       else  // otherwise clone the matrix
         {
           mVars[v] = newVal->clone();

           newVal = mVars[v];
         }
     }
   else
     {
       exists = false;
       if (newVal->getName() == 0)  // if anonymous
         mVars.insert(std::pair<std::string, Matrix *>(v, newVal));
       else
         {
           mVars.insert(std::pair<std::string, Matrix *>(v, newVal->clone()));
           newVal = mVars[v];
         }
     }

   varIter it = mVars.find(v);
   newVal->updateName((*it).first.c_str());
   return exists;
 }


Matrix* UserSpace :: getVar(const char *varName){
  std::string v(varName);

  if(mVars.find(v) == mVars.end())
    return NULL;

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

 void finalizeMatrix(vector<OM_SUPPORT_TYPE> &elements)
   {
   }
 
 
 Matrix *UserSpace::loadFrom(const char *filename, ...)
 {
   ifstream sfile(filename, ios::in);
   string line;   
   Matrix *curM;
   int state = 0;
   string varName;
   int rows(0), cols(0);
   vector<OM_SUPPORT_TYPE> elements;

   int curRows(0), curCols(0);
   int error = 0;
   while(getline(sfile, line))
     {
       if (line.size() == 0) 
         continue;

       if (state == 0) // expecting a new matrix
         {
           if (isComment(line))
             {
               curRows = 0;
               curCols = 0;
               if(containName(line))
                 {
                   varName = getName(line);
                   state = 0;
                 }
               else if (isRow(line))
                 {
                   rows = getRows(line);
                   state = 0;
                 }
               else if (isCol(line))
                 {
                   cols = getCols(line);
                   state = 0;
                 }
             }
           else
             {
               curCols = readDataLine(elements, line);
               curRows++;
               if (cols != 0 && cols != curCols)
                 {
                   error = 1;
                   break;
                 }
               state = 1;
             }
         }
       else if (state = 1) // in the middle of reading data
         {          
           if (isComment(line)) // a new matrix starts
             {
               if (curRows > 0)
                 {
                   if (rows != 0 && rows != curRows)
                     {
                       error = 1;
                       break;
                     }
                   // store the data
                   int dims[2];
                   dims[0] = curRows; 
                   dims[1] = curCols;
                   const OM_SUPPORT_TYPE *e = &elements[0];
                   Matrix *m = new Matrix(NULL, 2, dims, e);
                   if (varName.size() != 0)
                     updateVar(varName.c_str(), m);
                   else
                     assert(0); // todo
                 }
               curRows = 0;
               curCols = 0;
               varName = "";
               state = 0;
               elements.clear();
               // processing the current line
               if(containName(line))
                 {
                   varName = getName(line);
                   state = 0;
                 }
               else if (isRow(line))
                 {
                   rows = getRows(line);
                   state = 0;
                 }
               else if (isCol(line))
                 {
                   cols = getCols(line);
                   state = 0;
                 }

             }
           else
             {
               // read data
               curCols = readDataLine(elements, line);
               curRows++;
               if (cols != 0 && cols !=  curCols)
                 throw ExeException("Error loading the file\n");

               state = 1;
             }
         }
     }

   if (state = 1)
     {
       if (curRows > 0)
         {
           if (rows != 0 && rows != curRows)
             {
               error = 1;
             }
           else
             {
               // store the data
               int dims[2];
               dims[0] = curRows; dims[1] = curCols;
               Matrix *m = new Matrix(NULL, 2, dims, &elements[0]);
               if (varName.size() != 0)
                 updateVar(varName.c_str(), m);
               else
                 assert(0); // todo

             }
         }
     }
   sfile.close();
   if (error == 1)
     throw ExeException("Error loading the file.");
 }

 void UserSpace::saveMatrixTo(const char *filename, ...)
 {
   
 }


}
