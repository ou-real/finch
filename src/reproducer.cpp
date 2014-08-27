#include <finch/reproducer.hpp>

using namespace finch;

reproducer::reproducer(const uint16_t num_required_agents)
  : _num_required_agents(num_required_agents)
{
}

reproducer::~reproducer()
{
}

uint16_t reproducer::num_required_agents() const
{
  return _num_required_agents;
}
