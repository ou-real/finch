#ifndef _FINCH_MAZE_NO_MODIFIER_HPP_
#define _FINCH_MAZE_NO_MODIFIER_HPP_

#include "maze_modifier.hpp"

namespace finch
{
  class maze_no_modifier : public maze_modifier
  {
  public:
    virtual matrix2<uint16_t> modify(const matrix2<uint16_t> &in, const experimental_parameters &params) const { return in; }
  };
}

#endif