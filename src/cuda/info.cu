#include <cuda/info.hpp>
#include <iostream>

using namespace std;

void print_cuda_info()
{
  int gpu = 0;
  cudaDeviceProp gpuProp;
  cudaGetDevice(&gpu);
  cudaGetDeviceProperties(&gpuProp, gpu);
  cout << "GPU Information:" << endl;
  cout << "\tName: " << gpuProp.name << "(" << gpuProp.major << "." << gpuProp.minor << ")" << endl;
  cout << "\tMemory: " << gpuProp.totalGlobalMem << endl;
  cout << "\tMax Threads/Block: " << gpuProp.maxThreadsPerBlock << endl;
  cout << "\tECC Enabled: " << gpuProp.eccEnabled << endl;
}