#pragma once

#include <string>
namespace ONLINE_MATLAB{
  using namespace std;
class ParseException{
 public:
  explicit ParseException(const string &r)
    :mReason(r)
  {}

  const std::string &getReason() const
    {
      return mReason;
    }
  virtual void cleanUp()
  {
    // donothing by default
  }

 private:
  std::string mReason;
};

} // namespace
