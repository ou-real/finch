#include <finch/speciation_breeder.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/program_node_types.hpp>
#include <finch/builder.hpp>
#include <finch/matrix2.hpp>
#include <finch/option.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <fstream>
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
    return static_cast<double>(rand() % resolution) / static_cast<double>(resolution);
  }
}

speciation_breeder::speciation_breeder(const matrix2<uint16_t> &maze, const uint16_t goal_row, const uint16_t goal_col)
  : _maze(maze)
  , _goal_row(goal_row)
  , _goal_col(goal_col)
  , _debug_totals(_maze.rows(), _maze.columns())
{
}

population speciation_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
{
  using namespace std;
  
  population ret;
  tree_crossover_reproducer repr;
  builder random_builder;
  
  population mut_gen = generation;
  const double max_dist = sqrt(_maze.rows() * _maze.rows() + _maze.columns() * _maze.columns());
  
  for(auto it = mut_gen.begin(); it != mut_gen.end(); ++it)
  {
    if(it->chromosomes().size() == 1) continue;
    it->set_chromosomes(vector<double> { rand_normal() * max_dist });
  }
  
  matrix2<uint16_t> resource_dist(_maze.rows(), _maze.columns());
  
  const double scaling_factor = 10.0 / max_dist; // TODO: Non-const multiplier
  for(uint16_t r = 0; r < resource_dist.rows(); ++r)
  {
    for(uint16_t c = 0; c < resource_dist.columns(); ++c)
    {
      const int32_t dr = static_cast<int32_t>(_goal_row) - static_cast<int32_t>(r);
      const int32_t dc = static_cast<int32_t>(_goal_col) - static_cast<int32_t>(c);
      resource_dist.at(r, c) = (max_dist - sqrt(dr * dr + dc * dc)) * scaling_factor;
    }
  }
  
  static bool once = false;
  if(!once)
  {
    ofstream resource_dist_file("resource_dist.grad");
    serialize(resource_dist_file, resource_dist);
    resource_dist_file.close();
    once = true;
  }
  
  for(auto it = mut_gen.begin(); it != mut_gen.end();)
  {
    // cout << mut_gen.size() << endl;
    const program_state &c = it->final_state();
    uint16_t &v = resource_dist.at(c.row, c.col);
    if(!v)
    {
      it = mut_gen.erase(it);
      continue;
    }
    --v;
    ++it;
  }
  
  matrix2<vector<const agent *> > phenotypes(_maze.rows(), _maze.columns());
  for(const auto &a : mut_gen)
  {
    phenotypes.at(a.final_state().row, a.final_state().col).push_back(&a);
  }
  
  map<const agent *, vector<const agent *> > matches;
  for(const auto &a : mut_gen)
  {
    const vector<vector<const agent *> > plausible_mates =
      phenotypes.find_in_range(a.final_state().row, a.final_state().col,
        a.chromosomes()[0]);
    
    vector<const agent *> flattened_mates;
    for(const auto &v : plausible_mates) flattened_mates.insert(flattened_mates.end(), v.begin(), v.end());
    
    for(auto it = flattened_mates.begin(); it != flattened_mates.end();)
    {
      if(distance((*it)->final_state().col, (*it)->final_state().row,
        a.final_state().col, a.final_state().row) > (*it)->chromosomes()[0])
      {
        it = flattened_mates.erase(it);
        continue;
      }
      ++it;
    }
    
    matches[&a] = flattened_mates;
  }
  
  while(ret.size() < generation.size() && !mut_gen.empty())
  {
    population::size_type mi = rand() % mut_gen.size();
    const agent &mother = mut_gen[mi];
    const vector<const agent *> fathers = matches[&mother];
    if(fathers.empty()) continue;
    
    agent father = *fathers[rand() % fathers.size()];
    
    agent mutant  = random_builder.grow(program_types_set, 2, 5);
    mutant.set_chromosomes(vector<double> { rand_normal() * max_dist });
    
    agent child   = repr.reproduce(vector<agent> { mother, father })[0];
    child.set_chromosomes(vector<double> { (mother.chromosomes()[0] + father.chromosomes()[0]) / 2.0 });
    
    agent res     = repr.reproduce(vector<agent> { child, mutant }) [0];
    res.set_chromosomes(vector<double> { (child.chromosomes()[0] + mutant.chromosomes()[0]) / 2.0 });
    
    ret.push_back(res);
    
    // cout << "Created agent with dist " << res.chromosomes()[0] << endl;
  }
  
  
  
  return ret;
}
