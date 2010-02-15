#pragma once

#include "common.h"
#include "assert.h"
#include <sstream>
#include <iostream>
#include <string>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <stdio.h>
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

template <class T>
  bool from_char(T& t, 
                   const char* s, 
                   std::ios_base& (*f)(std::ios_base&))
  {
    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
  }

template <typename T>
 int readDataLine(vector<T> &element, std::string &line)
  {
    char mDelims[] = " ";
    char *token = strtok(const_cast<char *>(line.c_str()), &mDelims[0]);
    int count = 0;
    T t;
    while (token != NULL) 
      {
        if (from_char(t, token, std::dec))
          {
            count++;
            element.push_back(t);
          }
        token = strtok(NULL, &mDelims[0]);
      }
    return count;
  }

  bool isComment(const std::string &);
  bool containName(const std::string &);
  string getName(const std::string &);
  bool isData(const std::string &);

  bool isRow(const std::string &);
  bool isCol(const std::string &);
  int getRows(const std::string &);
  int getCols(const std::string &);

} // namespace

