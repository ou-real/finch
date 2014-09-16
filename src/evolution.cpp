#include <finch/evolution.hpp>
#include <finch/fitness_mapper.hpp>
#include <finch/builder.hpp>
#include <finch/breeder.hpp>
#include <finch/csv.hpp>
#include <finch/program_node_types.hpp>
#include <finch/expression_simplifier.hpp>

#ifdef USE_CUDA
#include <finch/cuda/cuda_evaluator.hpp>
#else
#include <finch/cpu/cpu_evaluator.hpp>
#endif

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <ratio>
#include <sstream>

using namespace finch;

evolution::evolution(const experimental_parameters &exp_params, const builder *const initial_population_builder,
  fitness_mapper *const fitness, breeder *const breed)
  : _exp_params(exp_params)
  , _initial_population_builder(initial_population_builder)
  , _fitness(fitness)
  , _breed(breed)
{
  
}

void evolution::evolve(const matrix2<uint16_t> &maze, const std::string &heatmap_prefix, csv *const out)
{
  using namespace std;
  using namespace std::chrono;
  
  out->clear();
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(2U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return;
  }
  
  program_state final_state;
  if(!maze.index_of(3U, final_state.row, final_state.col)) {
    cerr << "Couldn't find end" << endl;
    return;
  }
  
  const uint32_t max_generations = _exp_params["generations"];
  const uint32_t pop_size = _exp_params["generation_size"];
  const uint32_t growth_tree_min = _exp_params["growth_tree_min"];
  const uint32_t growth_tree_max = _exp_params["growth_tree_max"];
  // Build initial population
  cout << "Generating initial population (size: " << pop_size << ")... ";
  population pop;
  
  for(uint32_t i = 0; i < pop_size; ++i) {
    const node program = _initial_population_builder->grow(program_types_set, growth_tree_min, growth_tree_max);
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
  const csv::column_handle soln_handle = out->append_column("Solution Found");
  
  matrix2<uint16_t> heatmap_total(maze.rows(), maze.columns());
  
  vector<double> fitnesses;
  for(uint32_t i = 0; i < max_generations; ++i) {
    cerr << "generation " << i << ".";
    
    const auto start_time = high_resolution_clock::now();
    
    eval.evaluate(maze, pop, initial_state, 300, final_state);
    
    {
      matrix2<uint16_t> heatmap(maze.rows(), maze.columns());
      for(const auto &a : pop)
      {
        ++heatmap.at(a.final_state().row, a.final_state().col);
        ++heatmap_total.at(a.final_state().row, a.final_state().col);
      }
      stringstream heatmap_file;
      heatmap_file << heatmap_prefix << "-gen" << i << ".heatmap";
      ofstream heatmap_out(heatmap_file.str());
      serialize(heatmap_out, heatmap);
      heatmap_out.close();
    }
    
    cerr << ".";
    fitnesses = _fitness->map_all(pop);
    
    if(_fitness->inverted_fitness())
    {
      double max_fitness = 0.0;
      for(auto f  : fitnesses) max_fitness = max(f, max_fitness);
      for(auto &f : fitnesses) f = max_fitness - f;
    }
    
    if(i + 1 >= max_generations) break;
    
    double avg = 0.0;
    for(auto f : fitnesses) avg += f;
    avg /= fitnesses.size();
    
    out->append_data(gen_handle, to_string(i));
    // May change in the future. Currently constant.
    out->append_data(pop_size_handle, to_string(pop_size));
    out->append_data(avg_fit_handle, to_string(avg));
    
    bool soln = false;
    for(const auto &a : pop)
    {
      const program_state &i = final_state;
      const program_state &f = a.final_state();
      if(i.row == f.row && i.col == f.col)
      {
        cerr << "solution found!" << endl;
        soln = true;
        
      }
    }
    out->append_data(soln_handle, to_string(soln));
    if(soln) goto done;
    
    cerr << ".";
    // Mutate and reproduce
    vector<thread> breeders;
    const static uint8_t breeder_threads = 8;
    const static uint32_t agents_per_thread = pop.size() / breeder_threads;
    population subpops[breeder_threads];
    vector<double> subfits[breeder_threads];
    population breed_res[breeder_threads];
    for(uint8_t b = 0; b < breeder_threads; ++b)
    {
      const population::size_type begin = b       * agents_per_thread;
      population::size_type end         = (b + 1) * agents_per_thread;
      if(b + 1 == breeder_threads) end = pop.size();
      
      subpops[b] = population(pop.begin() + begin, pop.begin() + end);
      subfits[b] = vector<double>(fitnesses.begin() + begin, fitnesses.begin() + end);
    }
    
    for(uint8_t b = 0; b < breeder_threads; ++b)
    {
      breeders.emplace_back(thread([&breed_res, b, this, &subpops, &subfits] {
        breed_res[b] = _breed->breed(subpops[b], subfits[b]);
      }));
    }
    
    for(auto &t : breeders) t.join();
    
    pop.clear();
    for(auto &br : breed_res) pop.insert(pop.end(), br.begin(), br.end());
    
    const auto end_time = high_resolution_clock::now();
    
    milliseconds time_span = duration_cast<milliseconds>(end_time - start_time);
    
    out->append_data(gen_time_handle, to_string(time_span.count()));
    
    cerr << "done! (" << time_span.count() << " ms)" << endl;
  }
  
  done:
  
  {
    stringstream heatmap_total_file;
    heatmap_total_file << heatmap_prefix << "-total.heatmap";
    ofstream heatmap_total_out(heatmap_total_file.str());
    serialize(heatmap_total_out, heatmap_total);
    heatmap_total_out.close();
  }
  
  double best_fitness = 100000.0;
  size_t index = 0;
  for(size_t i = 0; i < fitnesses.size(); ++i)
  {
    if(fitnesses[i] > best_fitness) continue;
    best_fitness = fitnesses[i];
    index = i;
  }
  
  cout << "Best ind " << best_fitness << " (" << index << ")" << ". " << final_state.row << ", " << final_state.col
    << " vs " << pop[index].final_state().row << ", " << pop[index].final_state().col << endl;
  
  vector<uint32_t> outp;
  expression_simplifier simplifier(program_types_set);
  simplifier.simplify(pop[index].program()).write(outp);
  ofstream best_ind("best_ind.prog");
  if(best_ind.is_open())
  {
    uint32_t size = outp.size();
    best_ind.write(reinterpret_cast<char *>(&size), sizeof size);
    best_ind.write(reinterpret_cast<char *>(&outp[0]), outp.size() * sizeof(uint32_t));
    best_ind.close();
  }
  
  ofstream best_ind_dot("best_ind.dot");
  if(best_ind_dot.is_open())
  {
    pop[index].program().write_dot(best_ind_dot);
    best_ind_dot.close();
  }
  
  _fitness->emit_summary("final_fitness");
  
  // Report results
  
}