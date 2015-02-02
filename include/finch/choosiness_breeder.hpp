#ifndef _FINCH_CHOOSINESS_BREEDER_HPP_
#define _FINCH_CHOOSINESS_BREEDER_HPP_

#include "breeder.hpp"

#include <finch/matrix2.hpp>
#include <vector>

namespace finch
{
  class choosiness_breeder : public breeder
  {
  public:
    choosiness_breeder(const experimental_parameters &exp_params, const matrix2<uint16_t> &maze, const uint16_t goal_row, const uint16_t goal_col);
    
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;
    
  private:
    matrix2<uint16_t> _maze;
    mutable matrix2<std::vector<const agent *> > _phenotypes;
  };
}

#endif