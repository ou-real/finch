#ifndef _FINCH_EVALUATION_RESULTS_HPP_
#define _FINCH_EVALUATION_RESULTS_HPP_

#include <vector>
#include "program_state.hpp"

namespace finch
{
  class evaluation_results
  {
  public:
    typedef std::vector<program_state> program_state_vector;
    
    evaluation_results(const program_state_vector &end_states);
    
    const program_state_vector &end_states() const;
    
    program_state_vector::const_iterator begin() const;
    program_state_vector::const_iterator end() const;
    
  private:
    program_state_vector _end_states;
  };
}

#endif