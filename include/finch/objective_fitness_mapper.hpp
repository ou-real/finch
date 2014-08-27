#ifndef _FINCH_OBJECTIVE_FITNESS_MAPPER_HPP_
#define _FINCH_OBJECTIVE_FITNESS_MAPPER_HPP_

#include <finch/fitness_mapper.hpp>

namespace finch
{
  class objective_fitness_mapper : public fitness_mapper
  {
  public:
    objective_fitness_mapper(const program_state &initial_state);
    
    const program_state &initial_state() const;
    
    virtual double map(const program_state &final_state);
    
  private:
    program_state _initial_state;
  };
}

#endif