#pragma once
#include "common.h"
#include "UserSpace.h"
#include "FunctionList.h"
#include "CritiqueQueue.h"
//class UserSpace;
namespace ONLINE_MATLAB {
  // each daemon initiate a matlab object that processes 
  class Matrix;
  class Command;
  class Matlab {

  public:
    explicit Matlab(int gpuId);
    ~Matlab();

    void newUser(std::string name);
    void delUser(std::string name);
    UserSpace *getUser(std::string name);

    CritiqueQueue<Command *> *getCritiqueQueue()
      {
        return &mGpuCommandQueue;
      }
    //    typedef     std::map<std::string, Matrix *(*)(int, Matrix **)> FUNCTIONMAP;
    //    typedef     Matrix*(*FUNCTION)(int, Matrix **);    
    OM_FUNCTION getFunction(const std::string &);
  private:
    std::map<std::string, UserSpace*> mUsers;
    pthread_mutex_t mUserspaceLock;

    std::map<std::string, Matrix *(*)(int, int, Matrix **)> mFunctions; // TODO duplicate for different supporting type

    CritiqueQueue<Command *> mGpuCommandQueue;
    pthread_t mGpuThreadt;

    
  };
}// namespace 
