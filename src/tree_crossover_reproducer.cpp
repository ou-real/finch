#include <finch/tree_crossover_reproducer.hpp>

#include <algorithm>
#include <iostream>

using namespace finch;

tree_crossover_reproducer::tree_crossover_reproducer()
  : reproducer(2)
{
}

std::vector<agent> tree_crossover_reproducer::reproduce(const std::vector<agent> &agents) const
{
  using namespace std;
  
  vector<agent> ret;
  auto prog1 = agents[0].program(), prog2 = agents[1].program();

  node *const n1 = &prog1.find(rand() % prog1.num_nodes());
  node *const n2 = &prog2.find(rand() % prog2.num_nodes());

  swap(*n1, *n2);


  if(prog1.depth() <= 17) ret.push_back(agent(prog1));
  if(prog2.depth() <= 17) ret.push_back(agent(prog2));
  return ret;
}
