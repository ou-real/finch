#ifndef _FINCH_COMBINATION_FITNESS_MAPPER_HPP_
#define _FINCH_COMBINATION_FITNESS_MAPPER_HPP_

#include <finch/fitness_mapper.hpp>

namespace finch
{
  template<typename A, typename B>
  class combination_fitness_mapper : public fitness_mapper
  {
  public:
    combination_fitness_mapper(const program_state &initial_state)
      : fitness_mapper(initial_state)
      , _a(initial_state)
      , _b(initial_state)
    {
    }
    
    virtual std::vector<double> map_all(const std::vector<program_state> &final_states)
    {
      const std::vector<double> ar = _a.map_all(final_states);
      const std::vector<double> br = _b.map_all(final_states);
      const std::vector<double> r;
      
      auto ait = ar.begin();
      auto bit = br.begin();
      for(; ait != ar.end() && bit != br.end(); ++ait, ++bit) r.push_back(((*ait) + (*bit)) / 2.0);
      return r;
    }
    
    virtual double map(const program_state &final_state)
    {
      return (_a.map(final_state) + _b.map(final_state)) / 2.0;
    }
    
  private:
    A _a;
    B _b;
  };
}

#endif