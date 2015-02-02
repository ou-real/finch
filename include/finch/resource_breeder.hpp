#ifndef _FINCH_RESOURCE_BREEDER_HPP_
#define _FINCH_RESOURCE_BREEDER_HPP_

#include "breeder.hpp"

#include <finch/matrix2.hpp>
#include <vector>

namespace finch
{
  class resource_breeder : public breeder
  {
  public:
    resource_breeder(const experimental_parameters &exp_params, const matrix2<uint16_t> &maze, const uint16_t goal_row, const uint16_t goal_col);
    
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;
    
  private:
    matrix2<uint16_t> _maze;
    mutable matrix2<uint16_t> _debug_totals;
    uint16_t _goal_row;
    uint16_t _goal_col;
    matrix2<uint16_t> _precomp_resource_dist;
    mutable matrix2<uint16_t> _resource_dist;
  };
}

#endif