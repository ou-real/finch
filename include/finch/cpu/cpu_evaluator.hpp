#ifndef _FINCH_CPU_EVALUATOR_HPP_
#define _FINCH_CPU_EVALUATOR_HPP_

#include <finch/evaluator.hpp>
#include <finch/program_state.hpp>

namespace finch
{
  class cpu_evaluator
  {
  public:
    cpu_evaluator();
    
    void evaluate(const matrix2<uint16_t> &maze, population &generation,
      const program_state &initial_state, const uint32_t op_lim, const program_state &goal_state) const;
    
  };
}

#endif