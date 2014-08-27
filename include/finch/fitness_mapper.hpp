#ifndef _FINCH_FITNESS_MAPPER_HPP_
#define _FINCH_FITNESS_MAPPER_HPP_

#include "population.hpp"
#include "program_state.hpp"

#include <vector>

namespace finch
{
  class fitness_mapper
  {
  public:
    virtual std::vector<double> map_all(const population &generation);
    
    const program_state &initial_state() const;
    
    virtual double map(const program_state &final_state) = 0;
    
  private:
  };
}

#endif