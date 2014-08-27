#include <finch/fitness_mapper.hpp>

using namespace finch;

std::vector<double> fitness_mapper::map_all(const population &generation)
{
  using namespace std;
  vector<double> ret;
  population::const_iterator it = generation.begin();
  for(; it != generation.end(); ++it) ret.push_back(map((*it).final_state()));
  return ret;
}
