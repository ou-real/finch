#ifndef _FINCH_PROGRAM_STATE_HPP_
#define _FINCH_PROGRAM_STATE_HPP_

#include "geometry.hpp"

#include <stdint.h>

namespace finch
{
  struct program_state
  {
    cardinal_direction dir;
    uint16_t row;
    uint16_t col;
  };
}

#endif