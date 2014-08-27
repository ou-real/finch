#ifndef _FINCH_CUDA_EVALUATOR_HPP_
#define _FINCH_CUDA_EVALUATOR_HPP_

#include <finch/evaluator.hpp>
#include <finch/program_state.hpp>

namespace finch
{
  class cuda_evaluator : public evaluator
  {
  public:
    cuda_evaluator();
    
    void evaluate(const matrix2<uint16_t> &maze, population &generation,
      const program_state &initial_state, const uint32_t op_lim) const;
    
  };
}

#endif