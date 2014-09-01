#ifndef _CPU_PROGRAM_INTERPRETER_HPP_
#define _CPU_PROGRAM_INTERPRETER_HPP_

#include <finch/program_state.hpp>

void cpu_program_interpreter(const uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *offsets, uint32_t *programs, uint32_t program_index, finch::program_state state, uint32_t op_lim, finch::program_state *res);

#endif