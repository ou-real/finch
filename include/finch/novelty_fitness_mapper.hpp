#ifndef _FINCH_NOVELTY_FITNESS_MAPPER_HPP_
#define _FINCH_NOVELTY_FITNESS_MAPPER_HPP_

#include <finch/fitness_mapper.hpp>
#include <vector>
#include <finch/matrix2.hpp>

namespace finch
{
  class novelty_fitness_mapper : public fitness_mapper
  {
  public:
    novelty_fitness_mapper(const matrix2<uint16_t> &maze);
    virtual std::vector<double> map_all(const population &generation);
    virtual double map(const program_state &final_state);
    
    virtual bool inverted_fitness() const;
    
    virtual bool emit_summary(const std::string &tag) const;
    
  private:
    double evaluate(const program_state &state) const;
    matrix2<uint32_t> _history;
    uint32_t _total_occurrences;
  };
}

#endif