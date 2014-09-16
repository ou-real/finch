#ifndef _FINCH_BREEDER_HPP_
#define _FINCH_BREEDER_HPP_

#include "population.hpp"
#include "experimental_parameters.hpp"
#include <vector>

namespace finch
{
  class breeder
  {
  public:
    breeder(const experimental_parameters &exp_params);
    
    const experimental_parameters &exp_params() const;
    
    virtual ~breeder();
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const = 0;
    
  private:
    experimental_parameters _exp_params;
  };
}

#endif