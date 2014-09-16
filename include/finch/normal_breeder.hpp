#ifndef _FINCH_NORMAL_BREEDER_HPP_
#define _FINCH_NORMAL_BREEDER_HPP_

#include "breeder.hpp"

namespace finch
{
  class normal_breeder : public breeder
  {
  public:
    normal_breeder(const experimental_parameters &exp_params);
    
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;
    
  private:
    agent tournament_select(const population &generation, const std::vector<double> &fitnesses, const uint8_t size) const;
  };
}

#endif