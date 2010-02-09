#pragma once

#include "common.h"
#include "../include/Matrix.h"
#include "../include/PlotPair.h"
#include <sstream>
#include <iostream>

namespace ONLINE_MATLAB{
  using namespace std;
  

class OmGnuplot {
 public:
  OmGnuplot()
   {
   }

    /*    void dump(ifstream &))
    {
    }  */
  void addPair(PlotPair pp)
  {
    mPairs.push_back(pp);
  }

  void run();

  std::string getGraphFileName() const { return mGraphFile; }
 private:
    void writeToFile()
    {
      for (int i = 0; i != mPairs.size(); i++)
        mPairs[i].writeToFile();  // get the file name 
    }

    void createScript();
    void launchGnuplot();

    std::vector<PlotPair> mPairs;
    std::string mScriptFile;
    std::string mGraphFile;
    
};

} // namespace
