#include <finch/program_state.hpp>

using namespace finch;

program_state::program_state()
  : dir(cardinal_direction::east)
  , row(0xFFF)
  , col(0xFFF)
{
  
}

program_state::program_state(cardinal_direction dir, uint16_t row, uint16_t col)
  : dir(dir)
  , row(row)
  , col(col)
{
}