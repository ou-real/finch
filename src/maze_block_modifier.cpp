#include <finch/maze_block_modifier.hpp>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace finch;
using namespace std;



matrix2<uint16_t> maze_block_modifier::modify(const matrix2<uint16_t> &in, const experimental_parameters &params) const
{
  maze_block_modifier_digest::change c = _changes[_loc++];
  if(c.nil) return in;
  
  auto copy = in;
  swap(copy.at(c.from_y, c.from_x), copy.at(c.to_y, c.to_x));
  return copy;
}