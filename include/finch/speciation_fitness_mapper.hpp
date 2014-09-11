#ifndef _FINCH_SPECIATION_FITNESS_MAPPER_HPP_
#define _FINCH_SPECIATION_FITNESS_MAPPER_HPP_

#include "fitness_mapper.hpp"

namespace finch
{
  class speciation_fitness_mapper : public fitness_mapper
  {
  public:
    virtual double map(const program_state &final_state);
    virtual bool inverted_fitness() const;
    virtual bool emit_summary(const std::string &tag) const;
  };
}

#endif