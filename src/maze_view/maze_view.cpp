#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <finch/ui/ui_tools.hpp>

using namespace finch;

void usage(const char *const path)
{
  std::cout << path << " [file]" << std::endl;
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    usage(argv[0]);
    return 1;
  }
  
  std::ifstream in(argv[1]);
  if(!in.is_open()) {
    std::cerr << "Failed to open \"" << argv[1] << "\" for reading!" << std::endl;
    return 1;
  }
  
  matrix2<uint16_t> maze;
  deserialize(in, maze);
  
  in.close();
  
  return ui::show_matrix2(maze);
}