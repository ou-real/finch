#include <finch/maze_block_modifier_digest.hpp>

#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace finch;

static vector<maze_block_modifier_digest::change> gen_digest(matrix2<uint16_t> maze, const unsigned generations)
{
  vector<maze_block_modifier_digest::change> ret;
  
  maze_block_modifier_digest dig;
  for(unsigned i = 0; i < 2000; ++i)
  {
    auto r = dig.modify(maze, experimental_parameters());
    
    maze = get<0>(r);
    ret.push_back(get<1>(r));
  }
  
  return ret;
}

static void serialize(ostream &out, uint8_t data)
{
  out.write(reinterpret_cast<const char *>(&data), sizeof data);
}

static void serialize(ostream &out, matrix2<uint16_t>::index data)
{
  out.write(reinterpret_cast<const char *>(&data), sizeof data);
}

static void serialize(ostream &out, uint32_t data)
{
  out.write(reinterpret_cast<const char *>(&data), sizeof data);
}

static void serialize(ostream &out, const maze_block_modifier_digest::change &data)
{
  serialize(out, data.nil);
  serialize(out, data.from_x);
  serialize(out, data.from_y);
  serialize(out, data.to_x);
  serialize(out, data.to_y);
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

template<typename T>
static void serialize(ostream &out, const vector<T> &data)
{
  serialize(out, static_cast<uint32_t>(data.size()));
  for(const auto &t : data) serialize(out, t);
}

static void usage(const char *const path)
{
  cout << path << " generations maze1 maze2 ..." << endl;
}

int main(int argc, char *argv[])
{
  if(argc <= 2)
  {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  
  const int gens = stoi(argv[1]);
  if(gens <= 0)
  {
    cerr << "Generations must be > 0" << endl;
    return EXIT_FAILURE;
  }
  
  bool failures = false;
  for(int maze = 2; maze < argc; ++maze)
  {
    cout << "Generating digest for \"" << argv[maze] << "\"...";
    cout.flush();
    
    ifstream in(argv[maze]);
    
    if(!in.is_open())
    {
      cerr << "Failed to open \"" << argv[maze] << "\" for reading!" << endl;
      failures |= true;
      continue;
    }
    
    matrix2<uint16_t> mat;
    deserialize(in, mat);
    
    in.close();
    
    ofstream out((string(argv[maze]) + ".digest").c_str());
    
    if(!out.is_open())
    {
      cerr << "Failed to open \"" << argv[maze] << "\" for writing!" << endl;
      failures |= true;
      continue;
    }
    
    serialize(out, gen_digest(mat, gens));
    
    out.close();
    cout << "done!" << endl;
  }
  
  return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}