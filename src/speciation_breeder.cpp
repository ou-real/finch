#include <finch/speciation_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/program_node_types.hpp>
#include <finch/builder.hpp>
#include <finch/matrix2.hpp>
#include <finch/option.hpp>

#include <algorithm>

using namespace finch;

namespace
{
  inline double distance(const double ax, const double ay, const double bx, const double by)
  {
    const double dx = ax - bx;
    const double dy = ay - by;
    return sqrt(dx * dx + dy * dy);
  }
}

population speciation_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
{
  using namespace std;
  
  population ret;
  tree_crossover_reproducer repr;
  builder random_builder;
  matrix2<vector<agent> > phenotypes;
  for(const auto &a : generation)
  {
    phenotypes.at(a.final_state().row, a.final_state().col).push_back(a);
  }
  
  population mut_gen = generation;
  while(ret.size() < generation.size() && !mut_gen.empty())
  {
    population::size_type mi = rand() % mut_gen.size();
    const agent &mother = mut_gen[mi];
    const vector<vector<agent> > plausible_mates =
      phenotypes.find_in_range(mother.final_state().row, mother.final_state().col,
        mother.chromosomes()[0]);
    
    vector<agent> flattened_mates;
    for(const auto &v : plausible_mates)
    {
      flattened_mates.insert(flattened_mates.end(), v.begin(), v.end());
    }
    
    option<agent> father;
    while(!flattened_mates.empty())
    {
      const vector<agent>::size_type fi = rand() % flattened_mates.size();
      const auto &potential_father = flattened_mates[fi];
      
      if(distance(potential_father.final_state().col, potential_father.final_state().row,
        mother.final_state().col, mother.final_state().row) > potential_father.chromosomes()[0])
      {
        flattened_mates.erase(flattened_mates.begin() + fi);
        continue;
      }
      
      father = option<agent>(potential_father);
    }
    
    mut_gen.erase(mut_gen.begin() + mi);
    
    if(father.none()) continue;
    
    const agent mutant  = random_builder.grow(program_types_set, 5, 10);
    const agent child   = repr.reproduce(vector<agent> { mother, father.get() })[0];
    const agent res     = repr.reproduce(vector<agent> { child, mutant }) [0];
    ret.push_back(res);
  }
  return ret;
}
