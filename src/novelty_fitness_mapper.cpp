#include <finch/novelty_fitness_mapper.hpp>
#include <thread>

using namespace finch;

novelty_fitness_mapper::novelty_fitness_mapper()
{
}

std::vector<double> novelty_fitness_mapper::map_all(const population &generation)
{
  using namespace std;
  vector<program_state> states;
  for(const agent &a : generation) states.push_back(a.final_state());
  const static vector<program_state>::size_type evaluation_thread_size = 50UL;
  vector<program_state>::size_type i = 0;
  vector<double> res(states.size());
  vector<thread> threads;
  for(; i < states.size(); i += evaluation_thread_size) {
    threads.emplace_back(thread(&novelty_fitness_mapper::evaluate, this,
      ref(states), i, std::min(i + evaluation_thread_size, states.size()), ref(res)));
  }
  for(thread &t : threads) t.join();
  _history.insert(_history.end(), states.begin(), states.end());
  return res;
}

double novelty_fitness_mapper::map(const program_state &final_state)
{
  std::vector<double> res(1);
  evaluate(std::vector<program_state> { final_state }, 0, 1, res);
  _history.push_back(final_state);
  return 0.0;
}


void novelty_fitness_mapper::evaluate(const std::vector<program_state> &states, const std::vector<program_state>::size_type start,
  const std::vector<program_state>::size_type end, std::vector<double> &res)
{
  using namespace std;
  for(vector<program_state>::size_type i = start; i < end; ++i) {
    const program_state &current = states[i];
    double val = 0.0;
    
    for(const program_state &state : _history) {
      double sum = 0.0;
      sum += abs(static_cast<int32_t>(state.row) - static_cast<int32_t>(current.row));
      sum += abs(static_cast<int32_t>(state.col) - static_cast<int32_t>(current.col));
      val += sqrt(sum);
    }
    
    if(_history.size()) val /= _history.size();
    
    res[i] = val;
  }
}