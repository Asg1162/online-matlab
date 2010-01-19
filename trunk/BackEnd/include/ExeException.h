#pragma once

#include "ParseException.h"

namespace ONLINE_MATLAB{
  using namespace std;
  
  class ExeException : public ParseException{
 public:
  explicit ExeException(const string &r)
    :ParseException(r)
  {}

    virtual void cleanUp();
 private:
};

} // namespace
