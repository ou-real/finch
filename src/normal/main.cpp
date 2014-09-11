#include <finch/evolution.hpp>
#include <finch/builder.hpp>
#include <finch/csv.hpp>
#include <finch/objective_fitness_mapper.hpp>
#include <finch/novelty_fitness_mapper.hpp>
#include <finch/speciation_fitness_mapper.hpp>
#include <finch/normal_breeder.hpp>
#include <finch/speciation_breeder.hpp>
#include <finch/expression_simplifier.hpp>
#include <finch/program_node_types.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>

#include <json/json.h>

#ifdef USE_CUDA
#include <finch/cuda/info.hpp>
#endif

using namespace finch;
using namespace std;

bool kickoff(istream &in)
{
  using namespace Json;
  
  Value root;
  Reader reader;
  
  if(!reader.parse(in, root, false))
  {
    cerr << reader.getFormatedErrorMessages() << endl;
    return false;
  }
  
  const Value maze_value = root["maze"];
  const string name = root["name"].asString();
  const string maze_prefix = maze_value["prefix"].asString();
  const unsigned maze_first = maze_value["suffix_first"].asUInt();
  const unsigned maze_last = maze_value["suffix_last"].asUInt();
  
  const Value experiment = root["experiment"];
  const string exp_method = experiment["method"].asString();
  const Value exp_params = experiment["parameters"];
  const unsigned generation_size = exp_params["generation_size"].asUInt();
  const unsigned generations     = exp_params["generations"].asUInt();
  
  if(!generation_size)
  {
    cerr << "experiment/parameters/generation_size must be greater than 0. Aborting." << endl;
    return false;
  }
  
  if(!generations)
  {
    cerr << "experiment/parameters/generations must be greater than 0. Aborting." << endl;
    return false;
  }
  
  enum method
  {
    unknown_method = 0,
    objective_method,
    novelty_method,
    speciation_method
  } resolved_method = unknown_method;
  
  if     (exp_method == "objective")  resolved_method = objective_method;
  else if(exp_method == "novelty")    resolved_method = novelty_method;
  else if(exp_method == "speciation") resolved_method = unknown_method;
  
  if(resolved_method == unknown_method)
  {
    cerr << "Unknown experimental method \"" << exp_method << "\"" << endl;
    return false;
  }
  
  cout << "EXPERIMENTAL SUMMARY:" << endl
    << "  Name: " << name << endl
    << "  Maze:" << endl
    << "    Prefix: " << maze_prefix << endl
    << "    Suffix: " << "[" << maze_first << "-" << maze_last << "]" << endl
    << "  Experiment:" << endl
    << "    Method: " << exp_method << endl
    << "    Parameters: " << endl
    << "      Generation Size: " << generation_size << endl
    << "      Number of Generations: " << generations << endl
    << endl;
  
  for(unsigned i = maze_first; i <= maze_last; ++i)
  {
    stringstream s;
    s << name << "/" << maze_prefix << "-" << i << ".maze";
    
    ifstream maze_in(s.str());
    if(!maze_in.is_open())
    {
      cerr << "Unable to open maze \"" << s.str() << "\". Skipping evolution." << endl;
      continue;
    }
    
    matrix2<uint16_t> maze;
    deserialize(maze_in, maze);
    maze_in.close();
    
    program_state goal_state;
    if(!maze.index_of(3U, goal_state.row, goal_state.col)) {
      cerr << "Couldn't find end" << endl;
      return 1;
    }
    
    builder         p;
    breeder        *b = 0;
    fitness_mapper *f = 0;
  
    switch(resolved_method)
    {
    case objective_method:
      b = new normal_breeder;
      f = new objective_fitness_mapper(goal_state);
      break;
    case novelty_method:
      b = new normal_breeder;
      f = new novelty_fitness_mapper(maze);
      break;
    case speciation_method:
      b = new speciation_breeder(maze, goal_state.row, goal_state.col);
      f = new speciation_fitness_mapper;
      break;
    }
  
    stringstream evolve_log_name;
    time_t t = time(nullptr);
    tm tm = *localtime(&t);
    evolve_log_name << name << "/" << "evolve_log-" << exp_method << "-" << i << ".csv";
    stringstream heatmap_prefix;
    heatmap_prefix << name << "/" << exp_method << "-" << i;
    ofstream evolve_log(evolve_log_name.str().c_str());
    csv out;
    evolution e(generation_size, &p, f, b);
    e.evolve(maze, generations, heatmap_prefix.str(), &out);
    out.write(evolve_log);
    evolve_log.close();
  
    delete b;
    delete f;
  }
  
  return true;
}

int main(int argc, char *argv[])
{
  // cin.get();
  
  if(argc < 2) {
    cerr << argv[0] << " [config]" << endl;
    return 1;
  }
  
  ifstream file(argv[1]);
  if(!file.is_open()) {
    cerr << "Failed to open " << argv[1] << " for reading!" << endl;
    return 1;
  }
  
  if(!kickoff(file))
  {
    cerr << "Kickoff failed. Aborting." << endl;
    return 1;
  }
  
  file.close();
  
  return 0;
}