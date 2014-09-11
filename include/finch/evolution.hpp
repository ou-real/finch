#ifndef _FINCH_EVOLUTION_HPP_
#define _FINCH_EVOLUTION_HPP_

#include <stdint.h>
#include "matrix2.hpp"

namespace finch
{
  class builder;
  class fitness_mapper;
  class breeder;
  class csv;
  
  class evolution
  {
  public:
    evolution(const uint32_t pop_size, const builder *const initial_population_builder,
      fitness_mapper *const fitness, breeder *const breed);
    
    void evolve(const matrix2<uint16_t> &maze, const uint32_t max_generations, const std::string &heatmap_prefix, csv *const out);
    
  private:
    uint32_t _pop_size;
    const builder *_initial_population_builder;
    fitness_mapper *_fitness;
    breeder *_breed;
  };
}

#endif