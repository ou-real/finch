#ifndef _FINCH_EVALUATOR_HPP_
#define _FINCH_EVALUATOR_HPP_

#include "evaluation_results.hpp"
#include "population.hpp"
#include "matrix2.hpp"

namespace finch
{
  class evaluator
  {
  public:
    virtual ~evaluator();
    
    virtual void evaluate(const matrix2<uint16_t> &maze, population &generation,
      const program_state &initial_state, const uint32_t op_lim) const = 0;
  };
}

#endif