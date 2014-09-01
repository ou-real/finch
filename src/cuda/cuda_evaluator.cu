#ifdef USE_CUDA

#include <finch/cuda/cuda_evaluator.hpp>

#include "program_interpreter.hpp"

# warning remove
#include <iostream>

using namespace finch;

cuda_evaluator::cuda_evaluator()
{
  
}

void cuda_evaluator::evaluate(const matrix2<uint16_t> &maze, population &generation,
  const program_state &initial_state, const uint32_t op_lim) const
{
  using namespace std;
  vector<uint32_t> offsets;
  vector<uint32_t> programs;
  population::iterator git = generation.begin();
  for(; git != generation.end(); ++git) {
    const agent &current = *git;
    offsets.push_back(programs.size());
    current.program().write(programs);
  }

  const size_t maze_size = sizeof(uint16_t) * maze.rows() * maze.columns();
  uint16_t *device_maze = 0;
  cudaMalloc(&device_maze, maze_size);
  const uint16_t *const maze_data = maze.ptr();
  cudaMemcpy(device_maze, maze_data, maze_size, cudaMemcpyHostToDevice);
  
  vector<program_state> res(generation.size());
  
  const size_t programs_size = programs.size() * sizeof(uint32_t);
  uint32_t *device_programs = 0;
  cudaMalloc(&device_programs, programs_size);
  cudaMemcpy(device_programs, &programs[0], programs_size, cudaMemcpyHostToDevice);
  
  const size_t offsets_size = offsets.size() * sizeof(uint32_t);
  uint32_t *device_offsets = 0;
  cudaMalloc(&device_offsets, offsets_size);
  cudaMemcpy(device_offsets, &offsets[0], offsets_size, cudaMemcpyHostToDevice);
  
  
  const size_t res_size = generation.size() * sizeof(program_state);
  program_state *device_res = 0;
  cudaMalloc(&device_res, res_size);
  
  const size_t total_size = programs_size + offsets_size + res_size;
  
  cout << "Allocating " << static_cast<double>(total_size) / 1024.0 << " KiB on GPU" << endl;
  
  // FIXME: Rounding error
  uint32_t threads = 10;
  uint32_t blocks = ceil((float)generation.size() / threads);
  cudaEvent_t start_event;
  cudaEvent_t end_event;
  cudaEventCreate(&start_event);
  cudaEventCreate(&end_event);
  cudaEventRecord(start_event, 0);
  program_interpreter<<<blocks, threads>>>(device_maze, maze.rows(), maze.columns(),
    device_offsets, device_programs, initial_state, op_lim, device_res);
  cudaEventRecord(end_event, 0);
  
  cudaEventSynchronize(start_event);
  cudaEventSynchronize(end_event);
  
  float dt_ms;
  cudaEventElapsedTime(&dt_ms, start_event, end_event);
  
  cudaEventDestroy(start_event);
  cudaEventDestroy(end_event);
  
  // cout << "Time: " << dt_ms << " ms" << endl;
  
  cudaError_t err = cudaSuccess;
  if((err = cudaGetLastError()) != cudaSuccess) {
    cerr << "CUDA Error: " << cudaGetErrorString(err) << endl;
  }
  
  cudaMemcpy(&res[0], device_res, res_size, cudaMemcpyDeviceToHost);
  
  git = generation.begin();
  for(vector<program_state>::const_iterator it = res.begin();
    it != res.end() && git != generation.end(); ++it, ++git)
  {
    (*git).set_final_state(*it);
  }
  
  cudaFree(device_res);
  cudaFree(device_offsets);
  cudaFree(device_programs);
  cudaFree(device_maze);
}

#endif