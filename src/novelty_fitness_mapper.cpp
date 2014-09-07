#include <finch/novelty_fitness_mapper.hpp>
#include <thread>

using namespace finch;

novelty_fitness_mapper::novelty_fitness_mapper(const matrix2<uint16_t> &maze)
  : _history(maze.rows(), maze.columns())
{
}

std::vector<double> novelty_fitness_mapper::map_all(const population &generation)
{
  using namespace std;
  vector<double> res;
  for(const auto &a : generation) res.push_back(evaluate(a.final_state()));
  for(const auto &a : generation) ++_history.at(a.final_state().row, a.final_state().col);
  _total_occurrences += generation.size();
  return res;
}

double novelty_fitness_mapper::map(const program_state &final_state)
{
  using namespace std;
  const double ret = evaluate(final_state);
  ++_history.at(final_state.row, final_state.col);
  ++_total_occurrences;
  return ret;
}


double novelty_fitness_mapper::evaluate(const program_state &state) const
{
  if(!_total_occurrences) return 0.0;
  
  using namespace std;
  double val = 0.0;
  
  for(uint16_t row = 0; row < _history.rows(); ++row)
  {
    for(uint16_t col = 0; col < _history.columns(); ++col)
    {
      double sum = 0.0;
      uint32_t num = _history.at(row, col);
      sum += num * abs(static_cast<int32_t>(row) - static_cast<int32_t>(state.row));
      sum += num * abs(static_cast<int32_t>(col) - static_cast<int32_t>(state.col));
      val += sqrt(sum);
    }
  }
  
  val /= _total_occurrences;
  
  return val;
}