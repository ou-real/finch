#include <finch/program_node_types.hpp>
#include <finch/tree_crossover_reproducer.hpp>
#include <finch/node_crossover_reproducer.hpp>

#include <fstream>

using namespace finch;

int main(int argc, char *argv[])
{
  using namespace finch::types;
  
  srand(time(NULL));
  
  node n0(prog2);
  n0[0] = node(left);
  n0[1] = node(prog2);
  n0[1][0] = node(move);
  n0[1][1] = node(right);
  
  node n1(prog2);
  n1[0] = node(right);
  n1[1] = node(if_wall_ahead);
  n1[1][0] = node(left);
  n1[1][1] = node(prog2);
  n1[1][1][0] = node(left);
  n1[1][1][1] = node(move);
  
  agent a0(n0);
  agent a1(n1);
  
  node_crossover_reproducer rep;
  std::vector<agent> rets = rep.reproduce(std::vector<agent> {a0, a1});
  
  std::ofstream n0_before("n0_before.dot");
  std::ofstream n1_before("n1_before.dot");
  std::ofstream n0_after("n0_after.dot");
  std::ofstream n1_after("n1_after.dot");

  a0.program().write_dot(n0_before);
  a1.program().write_dot(n1_before);
  
  rets[0].program().write_dot(n0_after);
  rets[1].program().write_dot(n1_after);
  
  n0_before.close();
  n1_before.close();
  n0_after.close();
  n1_after.close();
  
  return 0;
}