#include <finch/builder.hpp>
#include <finch/program_node_types.hpp>
#include <fstream>
#include <sstream>

using namespace finch;

int main(int argc, char *argv[])
{
  using namespace std;
  builder b;
  
  for(uint32_t i = 0; i < 20; ++i) {
    stringstream name;
    name << "builder-" << i << ".dot";
    ofstream f(name.str().c_str());
    b.grow(program_types_set, 1, 10).write_dot(f);
    f.close();
  }
  
  return 0;
}