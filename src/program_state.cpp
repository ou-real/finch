#include <finch/program_state.hpp>

using namespace finch;

program_state::program_state()
  : dir(cardinal_direction::east)
  , row(0xFFF)
  , col(0xFFF)
{
  
}