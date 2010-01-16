#pragma once

#include "common.h"
#include "Atom.h"
#include <iostream>


namespace ONLINE_MATLAB {

  using namespace std;
class Command {

  public:
    //    Command();
 Command(const char * in, string &str, string &curUser):
    mInput(in),
      mOutput(str),
      mCurUser(curUser)
    {
      pthread_cond_init(&mCommandSig, NULL);
      pthread_mutex_init(&mCommandLock, NULL);
    }

    //    ~Command();

    void run() ;
    /*    char *getCurUser()
    {
      return mCurUser.c_str();
      }*/

    void signal()
    {
      pthread_cond_signal(&mCommandSig);
    }

    void lock()
    {
      pthread_mutex_lock(&mCommandLock); 
    }
    void unlock()
    {
      pthread_mutex_unlock(&mCommandLock);      
    }

    void wait()
    {
      pthread_cond_wait(&mCommandSig, &mCommandLock);
      unlock();
    }


  private:
    pthread_mutex_t mCommandLock;
    pthread_cond_t mCommandSig;
    const char *mInput;
    std::string &mOutput;
    std::string &mCurUser;
};


}// namespace 
