#ifndef _FINCH_SPECIATION_BREEDER_HPP_
#define _FINCH_SPECIATION_BREEDER_HPP_

#include "breeder.hpp"

namespace finch
{
  class speciation_breeder : public breeder
  {
  public:
    speciation_breeder(const std::shared_ptr<reproducer> &implementation);
    
    virtual bool is_breedable(const agent &_0, const agent &_1) const;
    virtual agent breed(const agent &_0, const agent &_1) const;
    
  private:
    
  };
}

#endif