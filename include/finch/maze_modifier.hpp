#ifndef _FINCH_MAZE_MODIFIER_HPP_
#define _FINCH_MAZE_MODIFIER_HPP_

#include "matrix2.hpp"
#include "experimental_parameters.hpp"
#include <cstdint>

namespace finch
{
  class maze_modifier
  {
  public:
    virtual ~maze_modifier() {}
    virtual matrix2<uint16_t> modify(const matrix2<uint16_t> &in, const experimental_parameters &params) const = 0;
  };
}

#endif