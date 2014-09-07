#ifndef _FINCH_SPECIATION_BREEDER_HPP_
#define _FINCH_SPECIATION_BREEDER_HPP_

#include "breeder.hpp"

namespace finch
{
  class speciation_breeder : public breeder
  {
  public:
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;

  };
}

#endif