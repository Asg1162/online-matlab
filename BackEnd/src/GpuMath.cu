// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <cutil_inline.h>

#include "GpuMathKernel.cu"


static const int max_thread = 2;
static const int max_block = 3;

void getDimension(int numElement, unsigned int *blockDimx, unsigned int *gridDimx, unsigned int *gridDimy)
{
  int numthread = numElement>>2;

  if (*blockDimx > max_thread)
    {
      *blockDimx = max_thread;
      *gridDimx = (numthread-1)/max_thread + 1;
      if (*gridDimx > max_block)
        {
          *gridDimy = (*gridDimx - 1)/max_block + 1;
          *gridDimx = max_block;
        }
    }
}

extern "C" void omgSin(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  //  printf("numthreads(%d) grid.x(%d), grid.y(%d), threads(%d)\n", numthread, grid.x, grid.y, threads.x);
  OmgSin_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgCos(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgCos_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgTan(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgTan_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgCot(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgCot_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgLog(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgLog_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgLog2(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgLog2_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgLog10(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgLog10_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}


extern "C" void omgExp(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgExp_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgAbs(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgAbs_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgFix(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgFix_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgCeil(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgCeil_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgFloor(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgFloor_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}

extern "C" void omgRound(int numElement, const float *bufferin, float *bufferout)
{
  dim3 threads(1,1,1);
  dim3 grid(1,1,1);
  getDimension(numElement, &(threads.x), &(grid.x), &(grid.y));
  OmgRound_kernel<<<grid, threads>>>(numElement, bufferin,bufferout);
}
