#include <finch/evolution.hpp>
#include <finch/fitness_mapper.hpp>
#include <finch/builder.hpp>
#include <finch/breeder.hpp>
#include <finch/csv.hpp>
#include <finch/maze_modifier.hpp>
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

namespace
{
  inline double rand_normal(const uint32_t resolution = 10000)
  {
    return static_cast<double>(rand() % resolution) / static_cast<double>(resolution);
  }
}

evolution::evolution(const experimental_parameters &exp_params, const builder *const initial_population_builder,
  fitness_mapper *const fitness, breeder *const breed, const maze_modifier *const modifier)
  : _exp_params(exp_params)
  , _initial_population_builder(initial_population_builder)
  , _fitness(fitness)
  , _breed(breed)
  , _modifier(modifier)
{
}

bool evolution::evolve(const matrix2<uint16_t> &maze, const std::string &heatmap_prefix, csv *const out)
{
  using namespace std;
  using namespace std::chrono;
  
  matrix2<uint16_t> localMaze(maze);
  
  out->clear();
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(2U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return false;
  }
  
  program_state final_state;
  if(!maze.index_of(3U, final_state.row, final_state.col)) {
    cerr << "Couldn't find end" << endl;
    return false;
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
  const csv::column_handle avg_ch_handle = out->append_column("Average Choosiness");
  
  matrix2<uint16_t> heatmap_total(maze.rows(), maze.columns());
  const double max_dist = sqrt(maze.rows() * maze.rows() + maze.columns() * maze.columns());
  for(auto it = pop.begin(); it != pop.end(); ++it)
  {
    if(it->chromosomes().size() == 1) continue;
    it->set_chromosomes(vector<double> { rand_normal() * max_dist });
  }
  
  vector<double> fitnesses;
  bool soln = false;
  for(uint32_t i = 0; i < max_generations; ++i) {
    cerr << "generation " << i << ".";
    
    const auto start_time = high_resolution_clock::now();
    
    eval.evaluate(localMaze, pop, initial_state, 300, final_state);
    
    // {
    //   matrix2<uint16_t> heatmap(maze.rows(), maze.columns());
    //   for(const auto &a : pop)
    //   {
    //     ++heatmap.at(a.final_state().row, a.final_state().col);
    //     ++heatmap_total.at(a.final_state().row, a.final_state().col);
    //   }
    //   stringstream heatmap_file;
    //   heatmap_file << heatmap_prefix << "-gen" << i << ".heatmap";
    //   ofstream heatmap_out(heatmap_file.str());
    //   serialize(heatmap_out, heatmap);
    //   heatmap_out.close();
    // }
    
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
    
    // TODO: Fix
    /*double avg_ch = 0.0;
    for(auto a : pop) avg_ch += a.chromosomes()[0];
    avg_ch /= pop.size();*/
    
    out->append_data(gen_handle, to_string(i));
    out->append_data(pop_size_handle, to_string(pop.size()));
    out->append_data(avg_fit_handle, to_string(avg));
    out->append_data(avg_ch_handle, to_string(0.0));
    
    for(const auto &a : pop)
    {
      const program_state &i = final_state;
      const program_state &f = a.final_state();
      if(i.row == f.row && i.col == f.col)
      {
        soln = true;
        break;
      }
    }
    out->append_data(soln_handle, to_string(soln));
    if(soln) goto done;
    
    cerr << ".";
    // Mutate and reproduce
    pop = _breed->breed(pop, fitnesses);
    
    const auto end_time = high_resolution_clock::now();
    
    milliseconds time_span = duration_cast<milliseconds>(end_time - start_time);
    
    out->append_data(gen_time_handle, to_string(time_span.count()));
    
    if(_modifier)
    {
      cerr << ".";
      // Modify maze
      localMaze = _modifier->modify(localMaze, _exp_params);
    }
    
    cerr << "done! (" << time_span.count() << " ms)" << endl;
  }
  
  done:
  
  cerr << "done!" << endl;
  
  {
    stringstream heatmap_total_file;
    heatmap_total_file << heatmap_prefix << "-total.heatmap";
    ofstream heatmap_total_out(heatmap_total_file.str());
    serialize(heatmap_total_out, heatmap_total);
    heatmap_total_out.close();
  }
  
  _fitness->emit_summary("final_fitness");
  
  // Report results
  return soln;
}

population evolution::evolve_dry(const matrix2<uint16_t> &maze, const std::string &heatmap_prefix, csv *const out)
{
  using namespace std;
  using namespace std::chrono;
  
  out->clear();
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(2U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return population();
  }
  
  program_state final_state;
  if(!maze.index_of(3U, final_state.row, final_state.col)) {
    cerr << "Couldn't find end" << endl;
    return population();
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
  const csv::column_handle avg_ch_handle = out->append_column("Average Choosiness");
  
  matrix2<uint16_t> heatmap_total(maze.rows(), maze.columns());
  const double max_dist = sqrt(maze.rows() * maze.rows() + maze.columns() * maze.columns());
  for(auto it = pop.begin(); it != pop.end(); ++it)
  {
    if(it->chromosomes().size() == 1) continue;
    it->set_chromosomes(vector<double> { rand_normal() * max_dist });
  }
  
  vector<double> fitnesses;
  bool soln = false;
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
    
    // TODO: Fix
    /*double avg_ch = 0.0;
    for(auto a : pop) avg_ch += a.chromosomes()[0];
    avg_ch /= pop.size();*/
    
    out->append_data(gen_handle, to_string(i));
    out->append_data(pop_size_handle, to_string(pop.size()));
    out->append_data(avg_fit_handle, to_string(avg));
    out->append_data(avg_ch_handle, to_string(0.0));
    
    for(const auto &a : pop)
    {
      const program_state &i = final_state;
      const program_state &f = a.final_state();
      if(i.row == f.row && i.col == f.col)
      {
        soln = true;
        break;
      }
    }
    out->append_data(soln_handle, to_string(soln));
    if(soln) goto done;
    
    cerr << ".";
    // Mutate and reproduce
    pop = _breed->breed(pop, fitnesses);
    
    const auto end_time = high_resolution_clock::now();
    
    milliseconds time_span = duration_cast<milliseconds>(end_time - start_time);
    
    out->append_data(gen_time_handle, to_string(time_span.count()));
    
    cerr << "done! (" << time_span.count() << " ms)" << endl;
  }
  
  done:
  
  cerr << "done!" << endl;
  
  // Report results
  return pop;
}

bool evolution::evolve_wet(const matrix2<uint16_t> &maze, const std::string &heatmap_prefix, population pop, csv *const out)
{
  using namespace std;
  using namespace std::chrono;
  
  out->clear();
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(2U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return false;
  }
  
  program_state final_state;
  if(!maze.index_of(3U, final_state.row, final_state.col)) {
    cerr << "Couldn't find end" << endl;
    return false;
  }
  
  const uint32_t max_generations = _exp_params["generations"];
  const uint32_t pop_size = _exp_params["generation_size"];
  const uint32_t growth_tree_min = _exp_params["growth_tree_min"];
  const uint32_t growth_tree_max = _exp_params["growth_tree_max"];
  
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
  const csv::column_handle avg_ch_handle = out->append_column("Average Choosiness");
  
  matrix2<uint16_t> heatmap_total(maze.rows(), maze.columns());
  const double max_dist = sqrt(maze.rows() * maze.rows() + maze.columns() * maze.columns());
  for(auto it = pop.begin(); it != pop.end(); ++it)
  {
    if(it->chromosomes().size() == 1) continue;
    it->set_chromosomes(vector<double> { rand_normal() * max_dist });
  }
  
  vector<double> fitnesses;
  bool soln = false;
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
    
    // TODO: Fix
    /*double avg_ch = 0.0;
    for(auto a : pop) avg_ch += a.chromosomes()[0];
    avg_ch /= pop.size();*/
    
    out->append_data(gen_handle, to_string(i));
    out->append_data(pop_size_handle, to_string(pop.size()));
    out->append_data(avg_fit_handle, to_string(avg));
    out->append_data(avg_ch_handle, to_string(0.0));
    
    for(const auto &a : pop)
    {
      const program_state &i = final_state;
      const program_state &f = a.final_state();
      if(i.row == f.row && i.col == f.col)
      {
        soln = true;
        break;
      }
    }
    out->append_data(soln_handle, to_string(soln));
    if(soln) goto done;
    
    cerr << ".";
    // Mutate and reproduce
    pop = _breed->breed(pop, fitnesses);
    
    const auto end_time = high_resolution_clock::now();
    
    milliseconds time_span = duration_cast<milliseconds>(end_time - start_time);
    
    out->append_data(gen_time_handle, to_string(time_span.count()));
    
    cerr << "done! (" << time_span.count() << " ms)" << endl;
  }
  
  done:
  
  cerr << "done!" << endl;
  
  _fitness->emit_summary("final_fitness");
  
  // Report results
  return soln;
}