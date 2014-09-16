#include <finch/normal_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
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
  builder random_builder;
  expression_simplifier es(program_types_set);
  experimental_parameters e = exp_params();
  const uint8_t tournament_size = e["tournament_size"];
  const uint8_t simp            = e["simplification_mod"];
  const uint32_t growth_tree_min = e["growth_tree_min"];
  const uint32_t growth_tree_max = e["growth_tree_max"];
  while(ret.size() < generation.size())
  {
    const agent &mother = tournament_select(generation, fitnesses, tournament_size);
    const agent &father = tournament_select(generation, fitnesses, tournament_size);
    if(mother.program().nonterminals() > 1000 || father.program().nonterminals() > 1000)
    {
      continue;
    }
    agent mutant  = random_builder.grow(program_types_set, growth_tree_min, growth_tree_max);
    mutant.set_program(es.simplify(mutant.program()));
    const agent child   = repr.reproduce(vector<agent> { mother, father })[rand() % 2];
    agent res     = repr.reproduce(vector<agent> { child, mutant })[rand() % 2];
    if(simp && rand() % simp == 0)
    {
      res.set_program(es.simplify(res.program()));
    }
    ret.push_back(res);
  }
  return ret;
}

agent normal_breeder::tournament_select(const population &generation,
  const std::vector<double> &fitnesses, const uint8_t size) const
{
  using namespace std;
  
  typedef tuple<agent, double> t;
  
  vector<t> candidates;
  for(uint8_t i = 0; i < size; ++i)
  {
    const population::size_type loc = rand() % generation.size();
    candidates.push_back(make_tuple(generation[loc], fitnesses[loc]));
  }
  
  sort(candidates.begin(), candidates.end(), [] (const t &l, const t &r)
    {
      return get<1>(l) < get<1>(r);
    });
  
  return get<0>(candidates[0]);
}