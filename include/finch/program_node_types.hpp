#ifndef _FINCH_PROGRAM_NODE_TYPES_HPP_
#define _FINCH_PROGRAM_NODE_TYPES_HPP_

#include "node_type.hpp"
#include "node_type_set.hpp"

namespace finch
{
  namespace types
  {
    extern const node_type left;
    extern const node_type right;
    extern const node_type move;
    extern const node_type if_wall_ahead;
    extern const node_type if_goal_visible;
    extern const node_type prog2;
    extern const node_type debug_point;
  }
  
  extern const node_type_set program_types_set;
}

#endif