#ifndef _FINCH_BREEDER_HPP_
#define _FINCH_BREEDER_HPP_

#include "population.hpp"
#include <vector>

namespace finch
{
  class breeder
  {
  public:
    virtual ~breeder();
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const = 0;
  };
}

#endif