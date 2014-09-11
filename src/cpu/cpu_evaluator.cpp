#include <finch/cpu/cpu_evaluator.hpp>

#include "cpu_program_interpreter.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace finch;

cpu_evaluator::cpu_evaluator()
{
}

void cpu_evaluator::evaluate(const matrix2<uint16_t> &maze, population &generation,
  const program_state &initial_state, const uint32_t op_lim) const
{
  
  using namespace std;
  using namespace std::chrono;
  vector<uint32_t> offsets;
  vector<uint32_t> programs;
  population::iterator git = generation.begin();
  for(; git != generation.end(); ++git) {
    const agent &current = *git;
    offsets.push_back(programs.size());
    current.program().write(programs);
  }
  
  vector<program_state> res(generation.size());
  
  const auto start_time = high_resolution_clock::now();
  
  const static uint32_t num_threads = thread::hardware_concurrency() << 1;
  
  const uint32_t agents_per_thread = generation.size() / num_threads;
  std::vector<thread> threads;
  for(uint32_t t = 0; t < num_threads; ++t)
  {
    const uint32_t start =  t      * agents_per_thread;
    uint32_t end         = (t + 1) * agents_per_thread;
    if(t + 1 == num_threads) end = generation.size();
    
    threads.emplace_back(thread([start, end, &maze, &offsets, &programs, &initial_state, &op_lim, &res]
      {
        for(uint32_t i = start; i < end; ++i)
        {
          matrix2<uint16_t> tmp = maze;
          cpu_program_interpreter(tmp.ptr(), tmp.rows(), tmp.columns(),
            &offsets[0], &programs[0], i, initial_state, op_lim, &res[0]);
        }
      }));
  }
  for(auto &th : threads) th.join();
  const auto end_time = high_resolution_clock::now();
  
  git = generation.begin();
  for(auto it = res.begin(); it != res.end() && git != generation.end(); ++it, ++git)
  {
    // cout << "final_state: " << it->row << ", " << it->col << endl; 
    (*git).set_final_state(*it);
  }
}