#include "../include/Gpu.h"
#include <cstdio>

int initGpu(int gpuId)
{
  culaStatus status;
  int numDevices;


  status = culaGetDeviceCount(&numDevices);
  if (status != culaNoError)  goto _exit;

  if (gpuId >= numDevices)  goto _exit_no_warning;

  printf("TODO bypassing select %dth device from total of %d.\n", gpuId, numDevices);
  //  status = culaSelectDevice(gpuId); // TODO 
  if (status != culaNoError)  goto _exit;

  status = culaInitialize();
  if(status != culaNoError) goto _exit;

  int boundto;
  status = culaGetExecutingDevice(&boundto);
  if(boundto != gpuId) goto _exit;

  return 0;

 _exit:
  checkStatus(status);
 _exit_no_warning:
  return 1;
  //  /* ... Your code ... */
}


void finalizeGpu()
{
  culaShutdown();
}


void checkStatus(culaStatus status)
{
  if(!status)
    return;

  if(status == culaArgumentError)
    printf("Invalid value for parameter %d\n", culaGetErrorInfo());
  else if(status == culaDataError)
    printf("Data error (%d)\n", culaGetErrorInfo());
  else if(status == culaBlasError)
    printf("Blas error (%d)\n", culaGetErrorInfo());
  else if(status == culaRuntimeError)
    printf("Runtime error (%d)\n", culaGetErrorInfo());
  else
    printf("%s\n", culaGetStatusString(status));
}

