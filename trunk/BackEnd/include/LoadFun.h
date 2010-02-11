#pragma once

#include "common.h"
#include "assert.h"
#include <sstream>
#include <iostream>
#include <string>
/* This base class encapsulates gpu memory manipulations
 */

namespace ONLINE_MATLAB {


template <class T>
  bool from_string(T& t, 
                   const std::string& s, 
                   std::ios_base& (*f)(std::ios_base&))
  {
    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
  }

  bool isComment(const std::string &);
  bool containName(const std::string &);
  string getName(const std::string &);
  

  bool isRow(const std::string &);
  bool isCol(const std::string &);
  int getRows(const std::string &);
  int getCols(const std::string &);

} // namespace

