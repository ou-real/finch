#ifndef _PROGRAM_INTERPRETER_HPP_
#define _PROGRAM_INTERPRETER_HPP_

#include <finch/program_state.hpp>

__global__ void program_interpreter(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *offsets, uint32_t *programs, finch::program_state state, uint32_t op_lim, finch::program_state *res);

#endif