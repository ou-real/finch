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
    
  private:
    bool select_node_crossover_point(const uint32_t max_tries, const node &prog1, uint32_t &final_loc1,
      const node &prog2, uint32_t &final_loc2) const;
  };
}

#endif