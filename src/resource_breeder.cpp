#include <finch/resource_breeder.hpp>
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
    return static_cast<double>(rand() % resolution) / static_cast<double>(resolution);
  }
}

resource_breeder::resource_breeder(const experimental_parameters &exp_params, const matrix2<uint16_t> &maze, const uint16_t goal_row, const uint16_t goal_col)
  : breeder(exp_params)
  , _maze(maze)
  , _goal_row(goal_row)
  , _goal_col(goal_col)
  , _debug_totals(_maze.rows(), _maze.columns())
  , _precomp_resource_dist(_maze.rows(), _maze.columns())
  , _resource_dist(_maze.rows(), _maze.columns())
{
  experimental_parameters e = exp_params;
  const double max_dist = sqrt(_maze.rows() * _maze.rows() + _maze.columns() * _maze.columns());
  const double scaling_factor = e["resource_levels"] / max_dist;
  for(uint16_t r = 0; r < _precomp_resource_dist.rows(); ++r)
  {
    for(uint16_t c = 0; c < _precomp_resource_dist.columns(); ++c)
    {
      const int32_t dr = static_cast<int32_t>(_goal_row) - static_cast<int32_t>(r);
      const int32_t dc = static_cast<int32_t>(_goal_col) - static_cast<int32_t>(c);
      _precomp_resource_dist.at(r, c) = (max_dist - sqrt(dr * dr + dc * dc)) * scaling_factor * 5;
    }
  }
  std::ofstream resource_dist_file("resource.dist");
  serialize(resource_dist_file, _precomp_resource_dist);
  resource_dist_file.close();
}

population resource_breeder::breed(const population &generation, const std::vector<double> &fitnesses) const
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
  
  _resource_dist = _precomp_resource_dist;
  const double max_dist = sqrt(_maze.rows() * _maze.rows() + _maze.columns() * _maze.columns());
  const auto check_res = [this] (const int16_t r, const int16_t c, const uint16_t res) -> bool
  {
    if(r < 0 || c < 0) return true;
    if(r > c >= _resource_dist.rows() || c >= _resource_dist.columns()) return true;
    
    uint16_t &v = _resource_dist.at(r, c);
    return v >= res;
  };
  
  const auto take_res = [this] (const int16_t r, const int16_t c, const uint16_t res)
  {
    if(r < 0 || c < 0) return;
    if(r > c >= _resource_dist.rows() || c >= _resource_dist.columns()) return;
    
    uint16_t &v = _resource_dist.at(r, c);
    v -= res;
  };
  
  uint32_t num_killed = 0;
  for(auto it = mut_gen.begin(); it != mut_gen.end();)
  {
    const program_state &c = (*it)->final_state();
    int16_t cr = c.row;
    int16_t cc = c.col;
    bool r = true;
    r &= check_res(cr, cc, 2);
    r &= check_res(cr + 1, cc, 1);
    r &= check_res(cr - 1, cc, 1);
    r &= check_res(cr, cc + 1, 1);
    r &= check_res(cr, cc - 1, 1);
    r &= check_res(cr + 1, cc - 1, 1);
    r &= check_res(cr - 1, cc - 1, 1);
    r &= check_res(cr + 1, cc + 1, 1);
    r &= check_res(cr - 1, cc + 1, 1);
    if(!r)
    {
      it = mut_gen.erase(it);
      ++num_killed;
      continue;
    }
    take_res(cr, cc, 2);
    take_res(cr + 1, cc, 1);
    take_res(cr - 1, cc, 1);
    take_res(cr, cc + 1, 1);
    take_res(cr, cc - 1, 1);
    take_res(cr + 1, cc - 1, 1);
    take_res(cr - 1, cc - 1, 1);
    take_res(cr + 1, cc + 1, 1);
    take_res(cr - 1, cc + 1, 1);
    ++it;
  }
  
  const population::size_type new_size = mut_gen.size() << 1;
  
  random_shuffle(mut_gen.begin(), mut_gen.end());
  while(ret.size() < new_size)
  {
    for(const auto &mother : mut_gen)
    {
      if(ret.size() >= new_size) break;
      
      const agent &father = *mut_gen[rand() % mut_gen.size()];
    
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
