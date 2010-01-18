#pragma once

#include "GpuCommand.hpp"

namespace ONLINE_MATLAB{
  class GpuTerminate:public GpuCommand{
  public:
    // default constructor and destructor are good.

  private:
    virtual int runCommand()
    {
      return 1;
    }
  };
} // namespace
