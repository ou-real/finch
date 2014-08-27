#ifndef _FINCH_BREEDER_HPP_
#define _FINCH_BREEDER_HPP_

#include "agent.hpp"
#include "reproducer.hpp"

#include <memory>

namespace finch
{
  class breeder
  {
  public:
    breeder(const std::shared_ptr<reproducer> &implementation);
    virtual ~breeder();
    
    const std::shared_ptr<reproducer> &implementation() const;
    
    virtual bool is_breedable(const agent &_0, const agent &_1) const = 0;
    virtual agent breed(const agent &_0, const agent &_1) const = 0;
    
  private:
    std::shared_ptr<reproducer> _implementation;
  };
}

#endif