#include <finch/fitness_mapper.hpp>

using namespace finch;

std::vector<double> fitness_mapper::map_all(const population &generation)
{
  using namespace std;
  vector<double> ret;
  for(const auto &a : generation) ret.push_back(map(a.final_state()));
  return ret;
}
