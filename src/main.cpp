#include <finch/evolution.hpp>
#include <finch/builder.hpp>
#include <finch/csv.hpp>
#include <finch/novelty_fitness_mapper.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

using namespace finch;
using namespace std;

int main(int argc, char *argv[])
{
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
  
  builder pop_builder;
  
  novelty_fitness_mapper novelty;
  
  
  stringstream evolve_log_name;
  time_t t = time(nullptr);
  tm tm = *localtime(&t);
  evolve_log_name << "evolve_log" << ".csv";
  ofstream evolve_log(evolve_log_name.str().c_str());
  csv out;
  evolution e(500, &pop_builder, &novelty);
  e.evolve(maze, 200, &out);
  out.write(evolve_log);
  evolve_log.close();
  
  return 0;
}