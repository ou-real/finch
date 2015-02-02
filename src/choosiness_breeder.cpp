#include <finch/choosiness_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/node_crossover_reproducer.hpp>
#include <finch/program_node_types.hpp>
#include <finch/builder.hpp>
#include <finch/matrix2.hpp>
#include <finch/option.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>
#include <deque>
#include <sstream>

using namespace finch;

namespace
{
  inline double distance(const double ax, const double ay, const double bx, const double by)
  {
    const double dx = ax - bx;
    const double dy = ay - by;
    return sqrt(dx * dx + dy * dy);
  }
  inline double rand_normal(const uint32_t resolution = 10000)
  {
    return static_cast<double>(rand() % resolution) / static_cast<double>(resolution)
      * static_cast<double>(rand() % resolution) / static_cast<double>(resolution);
  }
}

choosiness_breeder::choosiness_breeder(const experimental_parameters &exp_params, const matrix2<uint16_t> &maze,
    const uint16_t goal_row, const uint16_t goal_col)
  : breeder(exp_params)
  , _maze(maze)
  , _phenotypes(_maze.rows(), _maze.columns())
{
}

population choosiness_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
{
  using namespace std;
  
  population ret;
  tree_crossover_reproducer repr;
  node_crossover_reproducer mrepr;
  builder random_builder;
  
  deque<const agent *> mut_gen;
  for(const auto &a : generation) mut_gen.push_back(&a);
  
  experimental_parameters e = exp_params();
  const uint32_t growth_tree_min = e["growth_tree_min"];
  const uint32_t growth_tree_max = e["growth_tree_max"];
  
  const double max_dist = sqrt(_maze.rows() * _maze.rows() + _maze.columns() * _maze.columns());
  
  _phenotypes.clear();
  for(const auto &a : mut_gen) _phenotypes.at(a->final_state().row, a->final_state().col).push_back(a);
  
  map<const agent *, vector<const agent *> > matches;
  for(auto ait = mut_gen.begin(); ait != mut_gen.end();)
  {
    const agent *const a = *ait;
    const vector<vector<const agent *> > plausible_mates =
      _phenotypes.find_in_range(a->final_state().row, a->final_state().col,
        a->chromosomes()[0]);
    
    vector<const agent *> flattened_mates;
    for(const auto &v : plausible_mates) flattened_mates.insert(flattened_mates.end(), v.begin(), v.end());
    
    for(auto it = flattened_mates.begin(); it != flattened_mates.end();)
    {
      if(distance((*it)->final_state().col, (*it)->final_state().row,
        a->final_state().col, a->final_state().row) > (*it)->chromosomes()[0])
      {
        it = flattened_mates.erase(it);
        continue;
      }
      ++it;
    }
    
    if(flattened_mates.empty())
    {
      ait = mut_gen.erase(ait);
      continue;
    }
    
    matches[a] = flattened_mates;
    ++ait;
  }
  
  const population::size_type new_size = generation.size();
  
  random_shuffle(mut_gen.begin(), mut_gen.end());
  while(ret.size() < new_size)
  {
    for(const auto &mother : mut_gen)
    {
      if(ret.size() >= new_size) break;
    
      const vector<const agent *> &fathers = matches[mother];
      if(fathers.empty()) continue;
      
      const agent &father = *fathers[rand() % fathers.size()];
    
      agent mutant  = random_builder.grow(program_types_set, growth_tree_min, growth_tree_max);
      mutant.set_chromosomes(vector<double> { rand_normal() * max_dist });
    
      vector<agent> children   = repr.reproduce(vector<agent> { *mother, father });
      if(children.empty()) continue;
      agent &child = children[0];
      child.set_chromosomes(vector<double> { (mother->chromosomes()[0] + father.chromosomes()[0]) / 2.0 });
    
      agent res     = mrepr.reproduce(vector<agent> { child, mutant })[0];
      res.set_chromosomes(vector<double> { (child.chromosomes()[0] + mutant.chromosomes()[0]) / 2.0 });
    
      ret.push_back(res);
    }
  }
  
  return ret;
}
