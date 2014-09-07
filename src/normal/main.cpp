#include <finch/evolution.hpp>
#include <finch/builder.hpp>
#include <finch/csv.hpp>
#include <finch/objective_fitness_mapper.hpp>
#include <finch/novelty_fitness_mapper.hpp>
#include <finch/normal_breeder.hpp>
#include <finch/expression_simplifier.hpp>
#include <finch/program_node_types.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

#ifdef USE_CUDA
#include <finch/cuda/info.hpp>
#endif

using namespace finch;
using namespace std;

int main(int argc, char *argv[])
{
  cin.get();
  
  if(argc < 2) {
    cerr << argv[0] << " [maze]" << endl;
    return 1;
  }
  
  ifstream file(argv[1]);
  if(!file.is_open()) {
    cerr << "Failed to open " << argv[1] << " for reading!" << endl;
    return 1;
  }
  
  matrix2<uint16_t> maze;
  deserialize(file, maze);
  
  file.close();
  
#ifdef USE_CUDA
  print_cuda_info();
#endif
  
  builder pop_builder;
  
  
  
  novelty_fitness_mapper novelty(maze);
  normal_breeder breed;
  
  program_state goal_state;
  if(!maze.index_of(3U, goal_state.row, goal_state.col)) {
    cerr << "Couldn't find end" << endl;
    return 1;
  }
  
  cout << goal_state.row << ", " << goal_state.col << endl;
  
  objective_fitness_mapper objective(goal_state);
  
  
  stringstream evolve_log_name;
  time_t t = time(nullptr);
  tm tm = *localtime(&t);
  evolve_log_name << "evolve_log" << ".csv";
  ofstream evolve_log(evolve_log_name.str().c_str());
  csv out;
  evolution e(400, &pop_builder, &objective, &breed);
  e.evolve(maze, 200, &out);
  out.write(evolve_log);
  evolve_log.close();
  
  return 0;
}