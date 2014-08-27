#ifndef _FINCH_NOVELTY_FITNESS_MAPPER_HPP_
#define _FINCH_NOVELTY_FITNESS_MAPPER_HPP_

#include <finch/fitness_mapper.hpp>
#include <vector>

namespace finch
{
  class novelty_fitness_mapper : public fitness_mapper
  {
  public:
    novelty_fitness_mapper();
    virtual std::vector<double> map_all(const population &generation);
    virtual double map(const program_state &final_state);
    
  private:
    void evaluate(const std::vector<program_state> &states, const std::vector<program_state>::size_type start,
      const std::vector<program_state>::size_type end, std::vector<double> &res);
    std::vector<program_state> _history;
  };
}

#endif