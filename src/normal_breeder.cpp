#include <finch/normal_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/node_crossover_reproducer.hpp>
#include <finch/program_node_types.hpp>
#include <finch/expression_simplifier.hpp>
#include <finch/builder.hpp>

#include <algorithm>
#include <iostream>

using namespace finch;

normal_breeder::normal_breeder(const experimental_parameters &exp_params)
  : breeder(exp_params)
{
  
}

population normal_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
{
  using namespace std;  
  
  population ret;
  tree_crossover_reproducer repr;
  node_crossover_reproducer mrepr;
  builder random_builder;
  expression_simplifier es(program_types_set);
  experimental_parameters e = exp_params();
  const uint8_t tournament_size  = e["tournament_size"];
  const uint8_t simp             = e["simplification_mod"];
  const uint32_t growth_tree_min = e["growth_tree_min"];
  const uint32_t growth_tree_max = e["growth_tree_max"];
  while(ret.size() < generation.size())
  {
    const agent &mother = tournament_select(generation, fitnesses, tournament_size);
    const agent &father = tournament_select(generation, fitnesses, tournament_size);
    agent mutant      = random_builder.grow(program_types_set, growth_tree_min, growth_tree_max);
    const vector<agent> children = repr.reproduce(vector<agent> { mother, father });
    if(children.size() > 0)
    {
      agent res0        = mrepr.reproduce(vector<agent> { children[0], mutant })[0];
      ret.push_back(res0);
    }
    if(children.size() > 1)
    {
      agent res1        = mrepr.reproduce(vector<agent> { children[1], mutant })[0];
      ret.push_back(res1);
    }
  }
  return ret;
}

agent normal_breeder::tournament_select(const population &generation,
  const std::vector<double> &fitnesses, const uint8_t size) const
{
  using namespace std;
  
  const agent *best = 0;
  double best_fitness = 10000000.0;
  for(uint8_t i = 0; i < size; ++i)
  {
    const population::size_type loc = rand() % generation.size();
    if(fitnesses[loc] < best_fitness)
    {
      best = &generation[loc];
      best_fitness = fitnesses[loc];
    }
  }
  
  return *best;
}