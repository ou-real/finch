#ifndef _FINCH_RANDOM_BREEDER_HPP_
#define _FINCH_RANDOM_BREEDER_HPP_

#include "breeder.hpp"

#include <finch/matrix2.hpp>

namespace finch
{
  class random_breeder : public breeder
  {
  public:
    random_breeder(const experimental_parameters &exp_params);
    
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;
  };
}

#endif