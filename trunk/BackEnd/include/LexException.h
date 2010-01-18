#pragma once

#include "ParseException.h"

namespace ONLINE_MATLAB{
  using namespace std;
  
  class LexException : public ParseException{
 public:
  explicit LexException(const string &r)
    :ParseException(r)
  {}

 private:
};

} // namespace
