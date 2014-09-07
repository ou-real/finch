#include <finch/normal_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/program_node_types.hpp>
#include <finch/expression_simplifier.hpp>
#include <finch/builder.hpp>

#include <algorithm>
#include <iostream>

using namespace finch;

population normal_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
{
  // Pipe looks like this
  // 1. Tournament select two agents
  // 2. 
  using namespace std;  
  //
  // double accum = 0;
  // for(const auto &a : generation)
  // {
  //   accum += a.program().crowize().size();
  // }
  // accum /= generation.size();
  //
  // cout << "Average tree height: " << accum << endl;
  
  population ret;
  tree_crossover_reproducer repr;
  builder random_builder;
  expression_simplifier es(program_types_set);
  while(ret.size() < generation.size())
  {
    const agent &mother = tournament_select(generation, fitnesses, 10);
    const agent &father = tournament_select(generation, fitnesses, 10);
    agent mutant  = random_builder.grow(program_types_set, 2, 5);
    mutant.set_program(es.simplify(mutant.program()));
    const agent child   = repr.reproduce(vector<agent> { mother, father })[0];
    agent res     = repr.reproduce(vector<agent> { child, mutant }) [0];
    if(rand() % 4 == 0)
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