#include "Matlab.h"
#include <cassert>
#include "FunctionList.h"
#include "Gpu.h"
#include "Command.h"
#include "cublas.h"
#include <iostream>

namespace ONLINE_MATLAB{

  using namespace std;


extern "C" void 
  launchGpu_thread(void *q) 
  {
    int r = initGpu(0); // TODO pass gpuid 
    //    cublasInit();
    if (r != 0)
      {
        std::cout << "failed to initialize GPU, quitting...\n" << std::endl;
        exit(0);
      }
    cublasStatus cublasStatus;
    cublasStatus = cublasInit();
    if (cublasStatus != CUBLAS_STATUS_SUCCESS) {
      cout << "!!!! CUBLAS initialization error" << endl;
      return;
    }



    printf("inside gpu thread.\n");
    CritiqueQueue<Command *> *gpuQueue = (CritiqueQueue<Command *> *)q;
    int done = 0;
    while(1)
      {
        while(!gpuQueue->empty())
          {
            Command *command = gpuQueue->front();
            gpuQueue->pop();
            command->run();
            //            cutilSafeCall( cudaThreadSynchronize() ); // kernel launch is asnchronous, need to sync
            //            delete command;
          }
        if (done) break;
        gpuQueue->lock();
        gpuQueue->wait();
        gpuQueue->unlock();
      }
    cublasShutdown();
    printf("existing gpu thread.\n");
  }


Matlab :: Matlab(int gpuId){
  // build the function map
  mFunctions["rand"] = omSgerand;
  mFunctions["svd"] = omSgesvd;
  mFunctions["plot"] = omPlot;
  pthread_mutex_init(&mUserspaceLock, NULL);

  //  start a gpu thread
  pthread_create(&mGpuThreadt, NULL, (void *(*)(void*))launchGpu_thread, &mGpuCommandQueue);
}

Matlab :: ~Matlab(){
  std::map<std::string, UserSpace*>::iterator it;
  for(it=mUsers.begin(); it!=mUsers.end(); it++){
    delete it->second;
  }
  mUsers.clear();

}

void Matlab :: newUser(std::string name){
  // TODO more robust
  pthread_mutex_lock(&mUserspaceLock);
  
  if(mUsers.find(name) == mUsers.end())
    mUsers[name] = new UserSpace(this); 
  else
    mUsers[name]->incInstances();

  pthread_mutex_unlock(&mUserspaceLock);
}

void Matlab :: delUser(std::string name){
  pthread_mutex_lock(&mUserspaceLock);
  assert(mUsers[name]->getNumInstances() > 0); // TODO throw exception

  mUsers[name]->decInstances();
  if (mUsers[name]->getNumInstances() == 0)  // if instance reaches to zero, delete the userspace
    {
      delete(mUsers[name]);
      mUsers.erase(name);
    }
  pthread_mutex_unlock(&mUserspaceLock);
}

UserSpace* Matlab :: getUser(std::string name){
  pthread_mutex_lock(&mUserspaceLock);
  UserSpace *space = mUsers[name];
  pthread_mutex_unlock(&mUserspaceLock);
  return space;
}

 OM_FUNCTION Matlab :: getFunction(const std::string &funcName) 
   {
     // TODO throw exception
     assert(mFunctions.find(funcName) != mFunctions.end());

     return mFunctions[funcName];
   }

}