#include <finch/builder.hpp>

#include <iostream>

using namespace finch;

node builder::grow(const node_type_set &types, const uint32_t min_height, const uint32_t max_height) const
{
  
  // std::cout << min_height << " to " << max_height << std::endl;
  
  // Should this node be terminal?
  bool terminate = false;
  if(!min_height) terminate = max_height > 1 ? !(rand() % (max_height - 1)) : true;
  
  // Create a working set of node_types
  // that are either terminal or non-terminal
  node_type_set working;
  for(auto type : types) {
    if(terminate && !type.num_children()) working.push_back(type);
    if(!terminate && type.num_children()) working.push_back(type);
  }
  
  node_type self_type = working[rand() % working.size()];
  node self(self_type);
  
  uint32_t enforce_height = self_type.is_terminal() ? 0 : rand() % self_type.num_children();
  for(uint32_t i = 0; i < self_type.num_children(); ++i) {
    const node child = grow(types, enforce_height == i ? (min_height ? min_height - 1 : 0) : 0,
      enforce_height == i ? max_height - 1 : rand() % max_height);
    self[i] = child;
  }
  
  return self;
}