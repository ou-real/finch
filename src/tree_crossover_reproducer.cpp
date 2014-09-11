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
  
  auto total_slots = [] (const node &program) -> uint32_t
  {
    const uint32_t n = program.num_nodes() - 1; // Don't count root
    return (n * (n + 1)) >> 1;
  };
  
  auto slot_to_index = [] (const node &program, int64_t slot) -> uint32_t
  {
    uint32_t n = program.num_nodes() - 1;
    uint32_t ret = 0;
    while(slot >= 0)
    {
      ++ret;
      slot -= n--;
    }
    return ret;
  };
  
  auto prog1 = agents[0].program(), prog2 = agents[1].program();
  node *const n1 = &prog1.find(slot_to_index(prog1, rand() % total_slots(prog1)));
  node *const n2 = &prog2.find(slot_to_index(prog2, rand() % total_slots(prog2)));
  
  swap(*n1, *n2);
  
  return vector<agent> { agent(prog1), agent(prog2) };
}
