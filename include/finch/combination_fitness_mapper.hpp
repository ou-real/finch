#ifndef _FINCH_COMBINATION_FITNESS_MAPPER_HPP_
#define _FINCH_COMBINATION_FITNESS_MAPPER_HPP_

#include <finch/fitness_mapper.hpp>

namespace finch
{
  class combination_fitness_mapper : public fitness_mapper
  {
  public:
    combination_fitness_mapper(const double a_weight, fitness_mapper *const a, const double b_weight, fitness_mapper *const b)
      : _a_weight(a_weight)
      , _b_weight(b_weight)
      , _a(a)
      , _b(b)
    {
    }
    
    ~combination_fitness_mapper()
    {
      delete _a;
      delete _b;
    }
    
    virtual std::vector<double> map_all(const population &generation)
    {
      std::vector<double> ar = _a->map_all(generation);
      std::vector<double> br = _b->map_all(generation);
      std::vector<double> r;
      
      if(_a->inverted_fitness())
      {
        double max_fitness = 0.0;
        for(auto  f : ar) max_fitness = std::max(f, max_fitness);
        for(auto &f : ar) f = max_fitness - f;
      }
      if(_b->inverted_fitness())
      {
        double max_fitness = 0.0;
        for(auto  f : br) max_fitness = std::max(f, max_fitness);
        for(auto &f : br) f = max_fitness - f;
      }
      
      auto ait = ar.begin();
      auto bit = br.begin();
      for(; ait != ar.end() && bit != br.end(); ++ait, ++bit)
      {
        r.push_back(*ait * _a_weight + *bit * _b_weight);
      }
      return r;
    }
    
    virtual bool inverted_fitness() const
    {
      return false;
    }
    
    virtual double map(const program_state &final_state)
    {
      return _a->map(final_state) * _a_weight + _b->map(final_state) * _b_weight;
    }
    
    virtual bool emit_summary(const std::string &tag) const
    {
      return _a->emit_summary(tag + "_a") && _b->emit_summary(tag + "_b");
    }
    
  private:
    double _a_weight;
    double _b_weight;
    fitness_mapper *_a;
    fitness_mapper *_b;
  };
}

#endif