#include "../include/PlotPair.h"
#include "../include/ExeException.h"
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

 extern std::string gCurUser;
namespace ONLINE_MATLAB{

  using namespace std;
  

  static char filePath[] = "/home/xwu3/matcloud/mysite/media/users/";

const char *PlotPair::writeToFile()
{
  time_t seconds;

  seconds = time (NULL);
  mFileName = std::string(filePath);

  stringstream filename;
  filename << filePath << gCurUser << "/" << gCurUser << "_" << seconds <<  y << ".dat" ;
  cout << "user file name is "<< filename.str() << endl;
  mFileName = filename.str();
  

  ofstream file(mFileName.c_str(), ios::out);
  if (x != 0)
    {
      if (x->getBufferSize() != y->getBufferSize())
        {
          file.close();
          throw ExeException("Plot has un-matched x and y.");
        }
      OM_SUPPORT_TYPE *xptr = x->getInternalBuffer();
      OM_SUPPORT_TYPE *yptr = y->getInternalBuffer();
      for (int i = 0; i != x->getBufferSize(); i++)
        {
          file << *xptr++ << " " << *yptr++ << "\n" ;
        }
    }
  else
    {
      OM_SUPPORT_TYPE *yptr = y->getInternalBuffer();
      for (int i = 0; i != y->getBufferSize(); i++)
        {
          file << *yptr++ << "\n" ;
        }
    }

  file.close();
  return mFileName.c_str();
  
}


 void PlotPair::removeFile()
 {
   remove(mFileName.c_str());   
 }

 int PlotPair::getNbCol() const
 {
   if (x!= 0) 
     return 2;
   else
     return 1;
 }
} // namespace
