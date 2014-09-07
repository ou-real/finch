#ifndef _CPU_PROGRAM_INTERPRETER_HPP_
#define _CPU_PROGRAM_INTERPRETER_HPP_

#include <finch/program_state.hpp>

struct interpreter_state
{
  uint32_t *stack[255];
  uint8_t stack_head;
};

void cpu_stepper_init(interpreter_state *const state);

bool cpu_program_stepper(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *program, finch::program_state &state, uint32_t &op_lim, interpreter_state *const istate);

void cpu_program_interpreter(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *offsets, uint32_t *programs, uint32_t program_index, finch::program_state state, uint32_t op_lim, finch::program_state *res);

#endif