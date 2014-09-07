#ifndef _FINCH_TREE_CROSSOVER_REPRODUCER_HPP_
#define _FINCH_TREE_CROSSOVER_REPRODUCER_HPP_

#include "reproducer.hpp"

namespace finch
{
  class tree_crossover_reproducer : public reproducer
  {
  public:
    tree_crossover_reproducer();
    
    virtual std::vector<agent> reproduce(const std::vector<agent> &agents) const;
  };
}

#endif