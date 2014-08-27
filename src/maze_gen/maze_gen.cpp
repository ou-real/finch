#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <finch/maze_algorithms.hpp>

using namespace finch;

void usage(const char *const path)
{
  std::cout << path << " [file] [--rows value] [--columns value] [--min_dist value]"
    << "[--max_dist value] [--min_area value] [--rand_seed value] [--maze_count value]"
    << "[--with_border 0|1]" << std::endl;
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
  
  uint32_t rows = 50;
  uint32_t columns = 50;
  uint32_t min_dist = 0;
  uint32_t max_dist = UINT32_MAX;
  uint32_t min_area = 10;
  uint32_t rand_seed = time(NULL);
  uint32_t maze_count = 1;
  uint32_t with_border = 1;
  for(int32_t i = 2; i < argc; ++i) {
    const char *const current = argv[i];
    if(!strcmp("--rows", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for rows");
      rows = atoi(argv[++i]);
    } else if(!strcmp("--columns", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for columns");
      columns = atoi(argv[++i]);
    } else if(!strcmp("--min_dist", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for min_dist");
      min_dist = atoi(argv[++i]);
    } else if(!strcmp("--max_dist", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for max_dist");
      max_dist = atoi(argv[++i]);
    } else if(!strcmp("--min_area", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for min_area");
      min_area = atoi(argv[++i]);
    } else if(!strcmp("--rand_seed", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for rand_seed");
      rand_seed = atoi(argv[++i]);
    } else if(!strcmp("--maze_count", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for maze_count");
      maze_count = atoi(argv[++i]);
    } else if(!strcmp("--with_border", current)) {
      if(i + 1 >= argc) parse_failure("Missing value for with_border");
      with_border = atoi(argv[++i]);
      if(with_border != 1 && with_border != 0) parse_failure("with_border must be 0 or 1");
    }
  }
  
  using namespace std;
  
  cout << "Maze configuration:" << endl
    << "\t" << "rows: "     << rows << endl
    << "\t" << "columns: "  << columns << endl
    << "\t" << "min_dist: " << min_dist << endl
    << "\t" << "max_dist: " << max_dist << endl
    << "\t" << "min_area: " << min_area << endl
    << "\t" << "with_border: " << with_border << endl
    << "\t" << "rand_seed: " << rand_seed << endl
    << "\t" << "maze_count: " << maze_count << endl;
  
  srand(rand_seed);
  
  
  for(uint32_t i = 0; i < maze_count; ++i) {
    stringstream filename;
    filename << argv[1];
    if(maze_count > 1) filename << "-" << i;
    ofstream out(filename.str().c_str());
    if(!out.is_open()) {
      std::cerr << "Failed to open \"" << argv[1] << "\" for writing!" << std::endl;
      return 1;
    }
  
    matrix2<uint16_t> maze(rows, columns);
    if(with_border) maze.fill_border(1U);
    subdivide(maze, (uint16_t)0U, (uint16_t)1U, min_area, with_border);
  
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