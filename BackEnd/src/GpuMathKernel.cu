#pragma once

__global__ void
OmgSin_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = sinf(bufferin[gid]);
}


__global__ void
OmgCos_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = cosf(bufferin[gid]);
}


__global__ void
OmgTan_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = tanf(bufferin[gid]);
}


__global__ void
OmgCot_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = 1.0f/tanf(bufferin[gid]);
}


__global__ void
OmgLog_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = logf(bufferin[gid]);
}

__global__ void
OmgLog2_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = log2f(bufferin[gid]);
}

__global__ void
OmgLog10_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = log10f(bufferin[gid]);
}

__global__ void
OmgExp_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = expf(bufferin[gid]);
}

__global__ void
OmgAbs_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = fabsf(bufferin[gid]);
}

__global__ void
OmgFix_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = truncf(bufferin[gid]);
}

__global__ void
OmgCeil_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = ceilf(bufferin[gid]);
}

__global__ void
OmgFloor_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = floorf(bufferin[gid]);
}

__global__ void
OmgRound_kernel(int numElement, const float *bufferin, float *bufferout)
{
  // TODO make it u24
  int blockId = blockIdx.y * gridDim.x +  blockIdx.x;
  int tid = threadIdx.x + blockId * blockDim.x;
  int step = gridDim.x * blockDim.x;

    //blockIdx.y * step + blockIdx.x * gridDim.x * blockDim.x;
  for (int gid = tid; gid < numElement; gid += step)
    if (gid < numElement)
      bufferout[gid] = rintf(bufferin[gid]);
}
