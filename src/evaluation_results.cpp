#include <finch/evaluation_results.hpp>

using namespace finch;

evaluation_results::evaluation_results(const evaluation_results::program_state_vector &end_states)
  : _end_states(end_states)
{
}

const evaluation_results::program_state_vector &evaluation_results::end_states() const
{
  return _end_states;
}

evaluation_results::program_state_vector::const_iterator evaluation_results::begin() const
{
  return _end_states.begin();
}

evaluation_results::program_state_vector::const_iterator evaluation_results::end() const
{
  return _end_states.end();
}