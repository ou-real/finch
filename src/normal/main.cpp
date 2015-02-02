#include <finch/evolution.hpp>
#include <finch/builder.hpp>
#include <finch/csv.hpp>
#include <finch/objective_fitness_mapper.hpp>
#include <finch/novelty_fitness_mapper.hpp>
#include <finch/combination_fitness_mapper.hpp>
#include <finch/speciation_fitness_mapper.hpp>
#include <finch/normal_breeder.hpp>
#include <finch/random_breeder.hpp>
#include <finch/speciation_breeder.hpp>
#include <finch/resource_breeder.hpp>
#include <finch/choosiness_breeder.hpp>
#include <finch/maze_block_modifier.hpp>
#include <finch/expression_simplifier.hpp>
#include <finch/program_node_types.hpp>
#include <finch/experimental_parameters.hpp>
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

void test_novelty()
{
  matrix2<uint16_t> dummy(50, 50);
  novelty_fitness_mapper f(dummy);
  program_state p[] = {
    program_state(north, 0, 0),
    program_state(north, 49, 49),
    program_state(north, 0, 0),
  };
  for(const auto &pa : p) cout << f.map(pa) << endl;
  
  double max = 0.0;
  matrix2<uint16_t> m(dummy.rows(), dummy.columns());
  for(uint16_t r = 0; r < dummy.rows(); ++r)
  {
    for(uint16_t c = 0; c < dummy.columns(); ++c)
    {
      max = std::max(f.shadow_map(program_state(east, r, c)), max);
    }
  }
  for(uint16_t r = 0; r < dummy.rows(); ++r)
  {
    for(uint16_t c = 0; c < dummy.columns(); ++c)
    {
      m.at(r, c) = (max - f.shadow_map(program_state(east, r, c))) / max * 0xFFFF;
    }
  }
  
  ofstream mf("mf.grad");
  if(!mf.is_open()) return;
  serialize(mf, m);
  mf.close();
}

bool load_maze(const string &name, const string &maze_prefix, const unsigned i, matrix2<uint16_t> &maze, program_state &goal_state)
{
  stringstream s;
  s << name << "/" << maze_prefix << "-" << i << ".maze";
  
  ifstream maze_in(s.str());
  if(!maze_in.is_open())
  {
    cerr << "Unable to open maze \"" << s.str() << "\". Skipping evolution." << endl;
    return false;
  }
  
  deserialize(maze_in, maze);
  maze_in.close();
  
  if(!maze.index_of(3U, goal_state.row, goal_state.col)) {
    cerr << "Couldn't find end" << endl;
    return false;
  }
  
  return true;
}

static void deserialize(istream &in, uint8_t &data)
{
  in.read(reinterpret_cast<char *>(&data), sizeof data);
}

static void deserialize(istream &in, matrix2<uint16_t>::index &data)
{
  in.read(reinterpret_cast<char *>(&data), sizeof data);
}

static void deserialize(istream &in, uint32_t &data)
{
  in.read(reinterpret_cast<char *>(&data), sizeof data);
}

static void deserialize(istream &in, maze_block_modifier_digest::change &data)
{
  deserialize(in, data.nil);
  deserialize(in, data.from_x);
  deserialize(in, data.from_y);
  deserialize(in, data.to_x);
  deserialize(in, data.to_y);
}

template<typename T>
static void deserialize(istream &in, vector<T> &data)
{
  uint32_t size = 0;
  deserialize(in, size);
  data.resize(size);
  for(auto &t : data) deserialize(in, t);
}

// I'm so sorry for the poor soul
// who needs to refactor this.
bool kickoff(istream &in)
{
  srand(time(0));
  
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
  const bool multimaze           = exp_params["multimaze"].asBool();
  
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
  
  experimental_parameters e;
  for(auto it = exp_params.begin(); it != exp_params.end(); ++it) e[it.memberName()] = it->asDouble();
  
  enum method
  {
    unknown_method = 0,
    objective_method,
    novelty_method,
    combo_method,
    random_method,
    speciation_method,
    resource_method,
    choosiness_method,
  } resolved_method = unknown_method;
  
  if     (exp_method == "objective")   resolved_method = objective_method;
  else if(exp_method == "novelty")     resolved_method = novelty_method;
  else if(exp_method == "combination") resolved_method = combo_method;
  else if(exp_method == "random")      resolved_method = random_method;
  else if(exp_method == "speciation")  resolved_method = speciation_method;
  else if(exp_method == "resource")    resolved_method = resource_method;
  else if(exp_method == "choosiness")  resolved_method = choosiness_method;
  
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
    << "      Multimaze: " << multimaze << endl
    << "      Generation Size: " << generation_size << endl
    << "      Number of Generations: " << generations << endl
    << endl;
  
  uint32_t total_solved = 0;
  
  for(unsigned i = maze_first; multimaze ? (i < maze_last) : (i <= maze_last); ++i)
  {
    Value res = exp_params["resource_levels"];
    
    matrix2<uint16_t> maze;
    program_state goal_state;
  
    
    cout << endl << endl << "Evolution " << i << " (" << exp_method << ")" << endl << endl;
    
    builder p;
  
    stringstream evolve_log_name;
    time_t t = time(nullptr);
    tm tm = *localtime(&t);
    int obj_print = exp_params["objective_weight"].asDouble() * 10;
    int nov_print = exp_params["novelty_weight"].asDouble() * 10;
    evolve_log_name << name << "-exp3/" << "evolve_log-" << exp_method << "-" << generations << "-"
      << nov_print << obj_print << "-" << i;
    if(!!res) evolve_log_name << "-" << res.asString();
    evolve_log_name << ".csv";
    stringstream heatmap_prefix;
    heatmap_prefix << name << "-exp3/" << exp_method << "-" << generations << "-" << i;
    if(!!res) heatmap_prefix << "-" << res.asString();
    ofstream evolve_log(evolve_log_name.str().c_str());
    csv out;
    
    breeder        *b = 0;
    fitness_mapper *f = 0;
    maze_modifier  *m = 0;
    
    if(multimaze)
    {
      stringstream s;
      s << name << "/" << maze_prefix << "-" << i << ".maze.digest";
      ifstream digest(s.str());
      if(!digest.is_open())
      {
        cout << "Failed to load digest \"" << s.str() << "\"" << endl;
        return false;
      }
      
      vector<maze_block_modifier_digest::change> changes;
      deserialize(digest, changes);
      
      digest.close();
      
      m = new maze_block_modifier(changes);
    }
    
    load_maze(name, maze_prefix, i, maze, goal_state);
    
    switch(resolved_method)
    {
    case objective_method:
      b = new normal_breeder(e);
      f = new objective_fitness_mapper(goal_state);
      break;
    case novelty_method:
      b = new normal_breeder(e);
      f = new novelty_fitness_mapper(maze);
      break;
    case combo_method:
      b = new normal_breeder(e);
      f = new combination_fitness_mapper(
            exp_params["objective_weight"].asDouble(), new objective_fitness_mapper(goal_state),
            exp_params["novelty_weight"].asDouble()  , new novelty_fitness_mapper(maze));
    break;
    case random_method:
      b = new random_breeder(e);
      f = new objective_fitness_mapper(goal_state);
    break;
    case speciation_method:
      b = new speciation_breeder(e, maze, goal_state.row, goal_state.col);
      f = new objective_fitness_mapper(goal_state);
      break;
    case resource_method:
      b = new resource_breeder(e, maze, goal_state.row, goal_state.col);
      f = new objective_fitness_mapper(goal_state);
      break;
    case choosiness_method:
      b = new choosiness_breeder(e, maze, goal_state.row, goal_state.col);
      f = new objective_fitness_mapper(goal_state);
      break;
    default: return false;
    }
    
    evolution ev(e, &p, f, b, m);
    total_solved += ev.evolve(maze, heatmap_prefix.str(), &out) ? 1 : 0;

    delete b;
    delete f;
    delete m;
      
    out.write(evolve_log);
    evolve_log.close();
  }
  
  cout << endl << endl << "SUMMARY STATISTICS:" << endl
    << "\tTotal Evolutions: " << (maze_last - maze_first) << endl
    << "\tTotal Solutions Found: " << total_solved << endl;
  
  return true;
}

int main(int argc, char *argv[])
{
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