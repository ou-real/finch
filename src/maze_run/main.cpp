#include <finch/evaluator.hpp>
#include <finch/maze_algorithms.hpp>
#include <finch/objective_fitness_mapper.hpp>
#include <finch/cuda/cuda_evaluator.hpp>
#include <finch/program_node_types.hpp>
#include <finch/ui/ui_tools.hpp>

#include <iostream>
#include <ctime>
#include <fstream>

#include <QTimer>
#include <QThread>

using namespace finch;

int main(int argc, char *argv[])
{
  using namespace std;
  
  std::ifstream in(argv[1]);
  if(!in.is_open()) {
    std::cerr << "Failed to open \"" << argv[1] << "\" for reading!" << std::endl;
    return 1;
  }
  
  matrix2<uint16_t> maze;
  deserialize(in, maze);
  
  in.close();
  
  node program(types::if_wall_ahead);
  program[0] = node(types::right);
  program[1] = node(types::move);
  
  cout << program.sexpr() << endl;
  
  population pop;
  pop.push_back(agent(program));
  
  cout << maze << endl;
  
  program_state initial_state;
  initial_state.dir = east;
  if(!maze.index_of(3U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return 1;
  }
  
  cuda_evaluator ev;
  ev.evaluate(maze, pop, initial_state, 200);
  for(population::const_iterator it = pop.begin(); it != pop.end(); ++it)
  {
    cout << (*it).final_state().dir << endl;
  }
  
  program_state s = (*pop.begin()).final_state();
  maze.at(s.row, s.col) = 4;
  
  objective_fitness_mapper obj_fit(initial_state);
  std::vector<double> fitnesses = obj_fit.map_all(pop);
  cout << "Fitness: " << fitnesses[0] << endl;
  
  ui::show_matrix2(maze);
  
  return 0;
}