#ifndef _FINCH_MAZE_BLOCK_MODIFIER_HPP_
#define _FINCH_MAZE_BLOCK_MODIFIER_HPP_

#include "maze_modifier.hpp"
#include "maze_block_modifier_digest.hpp"

namespace finch
{
  class maze_block_modifier : public maze_modifier
  {
  public:
    maze_block_modifier(const std::vector<maze_block_modifier_digest::change> &changes) : _changes(changes) {}
    
    virtual matrix2<uint16_t> modify(const matrix2<uint16_t> &in, const experimental_parameters &params) const;
    
  private:
    std::vector<maze_block_modifier_digest::change> _changes;
    mutable uint32_t _loc = 0;
  };
}

#endif