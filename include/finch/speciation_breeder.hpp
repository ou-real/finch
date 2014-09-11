#ifndef _FINCH_SPECIATION_BREEDER_HPP_
#define _FINCH_SPECIATION_BREEDER_HPP_

#include "breeder.hpp"

#include <finch/matrix2.hpp>

namespace finch
{
  class speciation_breeder : public breeder
  {
  public:
    speciation_breeder(const matrix2<uint16_t> &maze, const uint16_t goal_row, const uint16_t goal_col);
    
    virtual population breed(const population &generation, const std::vector<double> &fitnesses) const;
    
  private:
    matrix2<uint16_t> _maze;
    mutable matrix2<uint16_t> _debug_totals;
    uint16_t _goal_row;
    uint16_t _goal_col;

  };
}

#endif