#ifndef _FINCH_BUILDER_HPP_
#define _FINCH_BUILDER_HPP_

#include "node.hpp"
#include "node_type_set.hpp"

namespace finch
{
  class builder
  {
  public:
    node grow(const node_type_set &types, const uint32_t min_height, const uint32_t max_height) const;
    
  private:
  };
}

#endif