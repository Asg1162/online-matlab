#include "Matrix.h"
#include "assert.h"
#include <iostream>
#include <cublas.h>
#include "stdio.h"
#include "../include/ExeException.h"
#include "../include/LoadFun.h"
#include <string.h>

namespace ONLINE_MATLAB{

using namespace std;


 bool isComment(const std::string &line)
 {
   string comment = "#";
   size_t found=line.find(comment);
   return (found!=string::npos);
 }

 bool containName(const std::string &line)
 {
   string name = "# name: ";
   size_t found=line.find(name);
   return (found!=string::npos);
 }

 /* bool isData(const std::string &line)
 {
   
 }*/

 string getName(const std::string &line)
 {
   string name = "# name: ";
   size_t found=line.find(name);
   string n = line.substr(found + name.size(), line.size());
   return n;
 }
  
 bool isRow(const std::string &line)
 {
   string rows = "# rows: ";
   size_t found=line.find(rows);
   return (found!=string::npos);

 }

 bool isCol(const std::string &line)
 {
   string columns = "# columns: ";
   size_t found=line.find(columns);
   return (found!=string::npos);
 }

 int getRows(const std::string &line)
 {
   string rows = "# rows: ";
   size_t found=line.find(rows);
   string n = line.substr(found + rows.size(), line.size()); 
   int row;
   if (from_string<int>(row, n, std::dec))
     return row;
   else
     return 0;

 }

 int getCols(const std::string &line)
 {
   string columns = "# columns: ";
   size_t found=line.find(columns);
   string n = line.substr(found + columns.size(), line.size());   
   int col;
   if (from_string<int>(col, n, std::dec))
     return col;
   else
     return 0;
 }
 

} // namespace
