#ifndef _FINCH_CPU_STEPPER_HPP_
#define _FINCH_CPU_STEPPER_HPP_

#include <finch/matrix2.hpp>
#include <finch/agent.hpp>
#include <finch/program_state.hpp>

namespace finch
{
  class cpu_stepper
  {
  public:
    cpu_stepper(const matrix2<uint16_t> &maze, uint32_t *const program, const program_state &state);
    ~cpu_stepper();
    
    const matrix2<uint16_t> &maze() const;
    const matrix2<uint16_t> &mod() const;
    const program_state &state() const;
    
    void reset();
    void step(uint32_t &op_lim);
    
  private:
    matrix2<uint16_t> _maze;
    matrix2<uint16_t> _mod;
    uint32_t *_program;
    program_state _initial_state;
    program_state _state;
    void *_istate;
  };
}

#endif