#include <finch/evaluator.hpp>
#include <finch/maze_algorithms.hpp>
#include <finch/objective_fitness_mapper.hpp>
#include <finch/program_node_types.hpp>
#include <finch/ui/ui_tools.hpp>
#include <finch/cpu/cpu_stepper.hpp>
#include <finch/maze_run/agent_stepper.hpp>

#include <iostream>
#include <ctime>
#include <fstream>

#include <QTimer>
#include <QThread>
#include <QApplication>

using namespace finch;
using namespace finch::ui;

int show_agent_stepper(const matrix2<uint16_t> &mat, uint32_t *const program)
{
  using namespace std;
  
  int argc = 0;
  char **argv = 0;
  QApplication app(argc, argv);
  agent_stepper stepper;
  program_state initial_state;
  initial_state.dir = east;
  if(!mat.index_of(2U, initial_state.row, initial_state.col)) {
    cerr << "Couldn't find start" << endl;
    return 1;
  }
  program_state final_state;
  final_state.dir = east;
  if(!mat.index_of(3U, final_state.row, final_state.col)) {
    cerr << "Couldn't find finish" << endl;
    return 1;
  }
  cpu_stepper cpu(mat, program, initial_state, final_state);
  stepper.resize(mat.columns() * 10, mat.rows() * 10);
  stepper.set_stepper(&cpu);
  stepper.show();
  stepper.raise();
  return app.exec();
}

int main(int argc, char *argv[])
{
  using namespace std;
  
  if(argc != 3) return 1;
  
  std::ifstream in(argv[1]);
  if(!in.is_open()) {
    std::cerr << "Failed to open \"" << argv[1] << "\" for reading!" << std::endl;
    return 1;
  }
  
  matrix2<uint16_t> maze;
  deserialize(in, maze);
  
  in.close();
  
  std::ifstream pin(argv[2]);
  if(!pin.is_open()) {
    std::cerr << "Failed to open \"" << argv[2] << "\" for reading!" << std::endl;
    return 1;
  }
  
  uint32_t size = 0;
  pin.read(reinterpret_cast<char *>(&size), sizeof size);
  uint32_t *const program = new uint32_t[size];
  pin.read(reinterpret_cast<char *>(program), size * sizeof(program[0]));
  
  pin.close();
  
  int ret = show_agent_stepper(maze, program);
  delete[] program;
  return ret;
}