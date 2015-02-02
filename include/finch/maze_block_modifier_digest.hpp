#ifndef _FINCH_MAZE_BLOCK_MODIFIER_DIGEST_HPP_
#define _FINCH_MAZE_BLOCK_MODIFIER_DIGEST_HPP_

#include <tuple>
#include "maze_modifier.hpp"

namespace finch
{
  class maze_block_modifier_digest
  {
  public:
    
    struct change
    {
      uint8_t nil;
      
      matrix2<uint16_t>::index from_x;
      matrix2<uint16_t>::index from_y;
      
      matrix2<uint16_t>::index to_x;
      matrix2<uint16_t>::index to_y;
    };
    
    virtual std::tuple<matrix2<uint16_t>, change> modify(const matrix2<uint16_t> &in, const experimental_parameters &params) const;
  };
}

#endif