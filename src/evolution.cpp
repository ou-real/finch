#include <finch/evolution.hpp>
#include <finch/fitness_mapper.hpp>
#include <finch/builder.hpp>
#include <finch/csv.hpp>
#include <finch/program_node_types.hpp>

#ifdef USE_CUDA
#include <finch/cuda/cuda_evaluator.hpp>
#else
#include <finch/cpu/cpu_evaluator.hpp>
#endif

#include <iostream>
#include <chrono>
#include <ratio>

using namespace finch;

evolution::evolution(const uint32_t pop_size, const builder *const initial_population_builder, fitness_mapper *const fitness)
  : _pop_size(pop_size)
  , _initial_population_builder(initial_population_builder)
  , _fitness(fitness)
{
  
}

void evolution::evolve(const matrix2<uint16_t> &maze, const uint32_t max_generations, csv *const out)
{
  using namespace std;
  using namespace std::chrono;
  
  out->clear();
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(3U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return;
  }
  
  // Build initial population
  cout << "Generating initial population (size: " << _pop_size << ")... ";
  population pop;
  for(uint32_t i = 0; i < _pop_size; ++i) {
    const node program = _initial_population_builder->grow(program_types_set, 1, 10);
    pop.emplace_back(agent(program));
  }
  cout << "done!" << endl;
  
  // Begin training
  
  cout << "Evolving for " << max_generations << " generations." << endl;
  
#ifdef USE_CUDA
  cuda_evaluator eval;
#else
  cpu_evaluator eval;
#endif
  
  const csv::column_handle gen_handle = out->append_column("Generation");
  const csv::column_handle pop_size_handle = out->append_column("Population Size");
  const csv::column_handle avg_fit_handle = out->append_column("Average Generation Fitness");
  const csv::column_handle gen_time_handle = out->append_column("Generation Time (ms)");
  
  for(uint32_t i = 0; i < max_generations; ++i) {
    cerr << "generation " << i << ".";
    
    const auto start_time = high_resolution_clock::now();
    
    eval.evaluate(maze, pop, initial_state, 200);
    
    vector<double> fitnesses = _fitness->map_all(pop);
    
    double avg = 0.0;
    for(auto f : fitnesses) avg += f;
    avg /= fitnesses.size();

    out->append_data(gen_handle, to_string(i));
    // May change in the future. Currently constant.
    out->append_data(pop_size_handle, to_string(_pop_size));
    out->append_data(avg_fit_handle, to_string(avg));
    
    
    // Mutate and reproduce
    const auto end_time = high_resolution_clock::now();
    
    milliseconds time_span = duration_cast<milliseconds>(end_time - start_time);
    
    out->append_data(gen_time_handle, to_string(time_span.count()));
    
    cerr << ". done! (" << time_span.count() << " ms)" << endl;
  }
  
  // Report results
  
}