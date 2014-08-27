#ifndef _FINCH_REPRODUCER_HPP_
#define _FINCH_REPRODUCER_HPP_

#include "agent.hpp"

#include <vector>
#include <stdint.h>

namespace finch
{
  class reproducer
  {
  public:
    reproducer(const uint16_t num_required_agents);
    virtual ~reproducer();
    
    uint16_t num_required_agents() const;
    virtual std::vector<agent> reproduce(const std::vector<agent> &agents) const = 0;
    
  private:
    uint16_t _num_required_agents;
  };
}

#endif