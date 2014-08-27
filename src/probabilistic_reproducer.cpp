#include <finch/probabilistic_reproducer.hpp>


using namespace finch;

probabilistic_reproducer::probabilistic_reproducer(const uint16_t num_required_agents,
  const std::vector<probabilistic_reproducer::reproducer_probability_pair> &reproducers)
  : reproducer(num_required_agents)
  , _reproducers(reproducers)
  , _cached_total(0U)
{
  using namespace std;
  for(auto pair : _reproducers) _cached_total += get<1>(pair);
}

std::vector<agent> probabilistic_reproducer::reproduce(const std::vector<agent> &agents) const
{
  using namespace std;
  
  shared_ptr<reproducer> rep;
  int64_t i = rand() % _cached_total;
  for(auto pair : _reproducers) {
    i -= get<1>(pair);
    if(i < 0) {
      rep = get<0>(pair);
      break;
    }
  }
  if(!rep) {
    // ERROR? Shouldn't happen.
    return std::vector<agent>();
  }
  
  return rep->reproduce(agents);
}