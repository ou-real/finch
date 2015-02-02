#include <finch/maze_block_modifier_digest.hpp>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <tuple>

using namespace finch;
using namespace std;

static void mark(const matrix2<uint16_t> &maze, uint16_t r, uint16_t c, matrix2<uint16_t> &marker)
{
  if(marker.at(r, c) == 1U) return;
  
  marker.at(r, c) = 1U;
  if(r < marker.rows() - 1) mark(maze, r + 1, c, marker);
  if(c < marker.columns() - 1) mark(maze, r, c + 1, marker);
  if(r > 0) mark(maze, r - 1, c, marker);
  if(c > 0) mark(maze, r, c - 1, marker);
}

tuple<matrix2<uint16_t>, maze_block_modifier_digest::change> maze_block_modifier_digest::modify(
  const matrix2<uint16_t> &in, const experimental_parameters &params) const
{
  {
    experimental_parameters::const_iterator it = params.find("block_modification_prob");
    maze_block_modifier_digest::change c;
    c.nil = 1;
    c.from_x = c.from_y = 0;
    c.to_x = c.to_y = 0;
    if(it != params.end() && (it->second * 100.0) < rand() % 100) return make_tuple(in, c);
  }
  
  typedef tuple<matrix2<uint16_t>::index, matrix2<uint16_t>::index> point;
  vector<point> candidates;
  vector<point> move_candidates;
  for(matrix2<uint16_t>::index r = 1; r < in.rows() - 1; ++r)
  {
    for(matrix2<uint16_t>::index c = 1; c < in.columns() - 1; ++c)
    {
      uint8_t walls = 0;
      if(in.at(r - 1, c)     == 1U) ++walls;
      if(in.at(r + 1, c)     == 1U) ++walls;
      if(in.at(r, c - 1)     == 1U) ++walls;
      if(in.at(r, c + 1)     == 1U) ++walls;
      if(in.at(r - 1, c - 1) == 1U) ++walls;
      if(in.at(r - 1, c + 1) == 1U) ++walls;
      if(in.at(r + 1, c - 1) == 1U) ++walls;
      if(in.at(r + 1, c + 1) == 1U) ++walls;
      
      if(in.at(r, c) == 1U)
      {
        if(walls > 0 && walls < 4) candidates.push_back(make_tuple(r, c));
      }
      else if(in.at(r, c) == 0U)
      {
        if(walls == 2) move_candidates.push_back(make_tuple(r, c));
      }
    }
  }
  
  if(candidates.empty() || move_candidates.empty())
  {
    maze_block_modifier_digest::change c;
    c.nil = 1;
    c.from_x = c.from_y = 0;
    c.to_x = c.to_y = 0;
    return make_tuple(in, c);
  }
  

  for(;;)
  {
    matrix2<uint16_t> ret(in);
    vector<point>::size_type fi = rand() % candidates.size();
    vector<point>::size_type mi = rand() % move_candidates.size();
    
    maze_block_modifier_digest::change c;
    c.nil = 0;
    c.from_x = get<1>(candidates[fi]);
    c.from_y = get<0>(candidates[fi]);
    c.to_x = get<1>(move_candidates[mi]);
    c.to_y = get<0>(move_candidates[mi]);
    
    uint16_t &val1 = ret.at(get<0>(candidates[fi]), get<1>(candidates[fi]));
    uint16_t &val2 = ret.at(get<0>(move_candidates[mi]), get<1>(move_candidates[mi]));
    
    
    swap(val1, val2);
    
    point start;
    for(matrix2<uint16_t>::index r = 1; r < in.rows() - 1; ++r)
    {
      for(matrix2<uint16_t>::index c = 1; c < in.columns() - 1; ++c)
      {
        if(ret.at(r, c) != 0U) continue;
        start = point(r, c);
        break;
      }
    }
    
    matrix2<uint16_t> marker(ret.rows(), ret.columns());
    
    for(matrix2<uint16_t>::index r = 0; r < ret.rows(); ++r)
    {
      for(matrix2<uint16_t>::index c = 0; c < ret.columns(); ++c)
      {
        if(ret.at(r, c) != 1U) start = point(r, c);
        else marker.at(r, c) = 1U;
      }
    }
    
    mark(ret, get<0>(start), get<1>(start), marker);
    
    bool failed = false;
    for(matrix2<uint16_t>::index r = 0; r < marker.rows(); ++r)
    {
      for(matrix2<uint16_t>::index c = 0; c < marker.columns(); ++c)
      {
        failed |= marker.at(r, c) == 0U;
      }
    }
    
    if(!failed) return make_tuple(ret, c);
  }
}