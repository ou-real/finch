#include <finch/cpu/cpu_stepper.hpp>
#include "cpu_program_interpreter.hpp"

#include <vector>
#include <iostream>

using namespace finch;

cpu_stepper::cpu_stepper(const matrix2<uint16_t> &maze, uint32_t *const program, const program_state &state, const program_state &goal)
  : _maze(maze)
  , _mod(maze)
  , _program(program)
  , _initial_state(state)
  , _goal_state(goal)
  , _istate(new interpreter_state)
{
  reset();
}

cpu_stepper::~cpu_stepper()
{
  delete reinterpret_cast<interpreter_state *>(_istate);
}

const program_state &cpu_stepper::state() const
{
  return _state;
}

const matrix2<uint16_t> &cpu_stepper::maze() const
{
  return _maze;
}

const matrix2<uint16_t> &cpu_stepper::mod() const
{
  return _mod;
}

void cpu_stepper::reset()
{
  _state = _initial_state;
  cpu_stepper_init(reinterpret_cast<interpreter_state *>(_istate));
}

void cpu_stepper::step(uint32_t &op_lim)
{
  using namespace std;
  _mod = _maze;
  cpu_program_stepper(_mod.ptr(), _maze.rows(), _maze.columns(),
    _program, _state, op_lim, reinterpret_cast<interpreter_state *>(_istate), _goal_state);
}