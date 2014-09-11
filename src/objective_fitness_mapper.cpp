#include <finch/objective_fitness_mapper.hpp>
#include <cmath>

using namespace finch;

objective_fitness_mapper::objective_fitness_mapper(const program_state &initial_state)
  : _initial_state(initial_state)
{
}

const program_state &objective_fitness_mapper::initial_state() const
{
  return _initial_state;
}

double objective_fitness_mapper::map(const program_state &final_state)
{
  const double ir = initial_state().row;
  const double ic = initial_state().col;
  const double fr = final_state.row;
  const double fc = final_state.col;
  
  const double dr = ir - fr;
  const double dc = ic - fc;
  
  // Distance formula
  return sqrt(dr * dr + dc * dc);
}

bool objective_fitness_mapper::inverted_fitness() const
{
  return false;
}

bool objective_fitness_mapper::emit_summary(const std::string &tag) const
{
  return true;
}