#ifndef _FINCH_PROBABILISTIC_REPRODUCER_HPP_
#define _FINCH_PROBABILISTIC_REPRODUCER_HPP_

#include "reproducer.hpp"

#include <tuple>
#include <memory>

namespace finch
{
  class probabilistic_reproducer : public reproducer
  {
  public:
    typedef std::tuple<std::shared_ptr<reproducer>, uint32_t> reproducer_probability_pair;
    
    probabilistic_reproducer(const uint16_t num_required_agents, const std::vector<reproducer_probability_pair> &reproducers);
    
    virtual std::vector<agent> reproduce(const std::vector<agent> &agents) const;
    
  private:
    std::vector<reproducer_probability_pair> _reproducers;
    uint32_t _cached_total;
  };
}

#endif