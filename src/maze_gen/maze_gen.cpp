#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <finch/maze_algorithms.hpp>
#include <json/json.h>
using namespace finch;
using namespace Json;
using namespace std;

using namespace finch;

void usage(const char *const path)
{
  std::cout << path << " [conf]" << std::endl;
}

void parse_failure(const char *const message)
{
  std::cerr << "Parse failure: " << message << std::endl;
  exit(1);
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    usage(argv[0]);
    return 1;
  }
  
  Value root;
  Reader reader;
  ifstream conf_file(argv[1]);
  if(!reader.parse(conf_file, root, false))
  {
    cerr << reader.getFormatedErrorMessages() << endl;
    conf_file.close();
    return 1;
  }
  conf_file.close();
  
  const Value maze_gen = root["maze_gen"];
  const Value maze_value = root["maze"];
  
  const string name = root["name"].asString();
  
  uint32_t rows = maze_gen["rows"].asUInt();
  uint32_t columns = maze_gen["columns"].asUInt();
  uint32_t min_dist = maze_gen["distance_min"].asUInt();
  uint32_t max_dist = maze_gen["distance_max"].asUInt();
  uint32_t min_area = maze_gen["area_min"].asUInt();
  uint32_t rand_seed = maze_gen["rand_seed"].asUInt();
  
  using namespace std;
  
  cout << "Maze configuration:" << endl
    << "\t" << "rows: "     << rows << endl
    << "\t" << "columns: "  << columns << endl
    << "\t" << "min_dist: " << min_dist << endl
    << "\t" << "max_dist: " << max_dist << endl
    << "\t" << "min_area: " << min_area << endl
    << "\t" << "rand_seed: " << rand_seed << endl;
  
  srand(rand_seed);
  
  const std::string maze_prefix = maze_value["prefix"].asString();
  const unsigned maze_first = maze_value["suffix_first"].asUInt();
  const unsigned maze_last = maze_value["suffix_last"].asUInt();
  
  for(uint32_t i = maze_first; i <= maze_last; ++i) {
    stringstream filename;
    filename << name << "/" << maze_prefix << "-" << i << ".maze";
    cout << "Generating \"" << filename.str() << "\"..." << endl;
    ofstream out(filename.str().c_str());
    if(!out.is_open()) {
      std::cerr << "Failed to open \"" << filename.str() << "\" for writing!" << std::endl;
      return 1;
    }
  
    matrix2<uint16_t> maze(rows, columns);
    maze.fill_border(1U);
    subdivide(maze, (uint16_t)0U, (uint16_t)1U, min_area, 1);
  
    point2<matrix2<uint16_t>::index> a;
    point2<matrix2<uint16_t>::index> b;
    uint32_t j = 0;
    
    for(; j < 1000; ++j) {
      // Start point
      a = random_location_satisfying(maze, (uint16_t)0U);
      maze.at(a.y(), a.x()) = 2U;
  
      // End point
      b = random_location_satisfying(maze, (uint16_t)0U);
      maze.at(b.y(), b.x()) = 3U;
      
      std::vector<point2<matrix2<uint16_t>::index> > p = path(maze, a, b);
      if(p.size() >= min_dist && p.size() <= max_dist) break;
      
      maze.at(a.y(), a.x()) = 0U;
      maze.at(b.y(), b.x()) = 0U;
    }
    
    if(j >= 1000) {
      cerr << "FAILED to create satisfactory path for " << filename.str() << " (tried " << j << " times)" << endl;
    }
    
    serialize(out, maze);
  
    out.close();
  }
  
  return 0;
}