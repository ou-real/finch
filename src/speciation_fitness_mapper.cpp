#include <finch/speciation_fitness_mapper.hpp>

using namespace finch;

double speciation_fitness_mapper::map(const program_state &final_state)
{
  return 0.0;
}

bool speciation_fitness_mapper::inverted_fitness() const
{
  return false;
}

bool speciation_fitness_mapper::emit_summary(const std::string &tag) const
{
  return true;
}