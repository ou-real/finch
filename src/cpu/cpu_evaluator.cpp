#include <finch/cpu/cpu_evaluator.hpp>

#include "cpu_program_interpreter.hpp"

#include <iostream>
#include <chrono>

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
  for(uint32_t i = 0; i < generation.size(); ++i)
  {
    cpu_program_interpreter(maze.ptr(), maze.rows(), maze.columns(),
      &offsets[0], &programs[0], i, initial_state, op_lim, &res[0]);
  }
  const auto end_time = high_resolution_clock::now();
  
  git = generation.begin();
  for(vector<program_state>::const_iterator it = res.begin();
    it != res.end() && git != generation.end(); ++it, ++git)
  {
    (*git).set_final_state(*it);
  }
}