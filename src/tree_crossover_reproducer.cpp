#include <finch/tree_crossover_reproducer.hpp>

using namespace finch;

tree_crossover_reproducer::tree_crossover_reproducer()
  : reproducer(2)
{
}

std::vector<agent> tree_crossover_reproducer::reproduce(const std::vector<agent> &agents) const
{
  node n1 = agents[0].program(), n2 = agents[1].program();
  uint32_t loc1 = 0, loc2 = 0;
  if(!select_tree_crossover_point(200, n1, loc1, n2, loc2)) return std::vector<agent>();
  std::swap(n1.find(loc1), n2.find(loc2));
  agent a1(n1), a2(n2);
  return std::vector<agent> { a1, a2 };
}

bool tree_crossover_reproducer::select_tree_crossover_point(const uint32_t max_tries,
  const node &prog1, uint32_t &final_loc1, const node &prog2, uint32_t &final_loc2) const
{
  for(uint32_t i = 0; i < max_tries; ++i) {
    final_loc1 = rand() % prog1.num_nodes();
    final_loc2 = rand() % prog2.num_nodes();
    const node &node1 = prog1.cfind(final_loc1);
    const node &node2 = prog2.cfind(final_loc2);
    // if(node1.type().similar_to(node2.type())) return true;
    return true;
  }
  return false;
}