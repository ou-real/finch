#ifndef _FINCH_EVOLUTION_HPP_
#define _FINCH_EVOLUTION_HPP_

#include <stdint.h>
#include "matrix2.hpp"
#include "experimental_parameters.hpp"

namespace finch
{
  class builder;
  class fitness_mapper;
  class breeder;
  class csv;
  
  class evolution
  {
  public:
    evolution(const experimental_parameters &exp_params, const builder *const initial_population_builder,
      fitness_mapper *const fitness, breeder *const breed);
    
    bool evolve(const matrix2<uint16_t> &maze, const std::string &heatmap_prefix, csv *const out);
    
  private:
    experimental_parameters _exp_params;
    const builder *_initial_population_builder;
    fitness_mapper *_fitness;
    breeder *_breed;
  };
}

#endif