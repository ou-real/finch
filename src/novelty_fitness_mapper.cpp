#include <finch/novelty_fitness_mapper.hpp>
#include <thread>
#include <fstream>
#include <iostream>

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

double novelty_fitness_mapper::shadow_map(const program_state &final_state) const
{
  return evaluate(final_state);
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
      sum += abs(static_cast<int32_t>(row) - static_cast<int32_t>(state.row))
          +  abs(static_cast<int32_t>(col) - static_cast<int32_t>(state.col));
      val += sqrt(_history.at(row, col) * sum);
    }
  }
  
  val /= _total_occurrences;
  
  return val;
}

bool novelty_fitness_mapper::inverted_fitness() const
{
  return true;
}

bool novelty_fitness_mapper::emit_summary(const std::string &tag) const
{
  using namespace std;
  
  uint32_t max = 0;
  for(uint16_t row = 0; row < _history.rows(); ++row)
  {
    for(uint16_t col = 0; col < _history.columns(); ++col)
    {
      max = std::max(max, _history.at(row, col));
    }
  }
  
  const double scaling_factor = static_cast<double>(0xFFFF) / max;
  
  matrix2<uint16_t> out(_history.rows(), _history.columns());
  for(uint16_t row = 0; row < _history.rows(); ++row)
  {
    for(uint16_t col = 0; col < _history.columns(); ++col)
    {
      cout << _history.at(row, col) << " ";
      out.at(row, col) = _history.at(row, col) * scaling_factor;
    }
    cout << endl;
  }
  
  ofstream file(tag + ".grad");
  if(!file.is_open()) return false;
  serialize(file, out);
  file.close();
  return true;
}