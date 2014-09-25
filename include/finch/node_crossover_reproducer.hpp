#ifndef _FINCH_NODE_CROSSOVER_REPRODUCER_HPP_
#define _FINCH_NODE_CROSSOVER_REPRODUCER_HPP_

#include "reproducer.hpp"

namespace finch
{
  class node_crossover_reproducer : public reproducer
  {
  public:
    node_crossover_reproducer();
    
    virtual std::vector<agent> reproduce(const std::vector<agent> &agents) const;
  };
}

#endif