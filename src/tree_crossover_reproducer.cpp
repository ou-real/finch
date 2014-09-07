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
  
  /*auto find_max = [] (const vector<vector<node *>> &rows)
  {
    size_t max = 0;
    for(size_t i = 0; i < rows.size(); ++i) max += (i + 1) * rows[rows.size() - i - 1].size();
    return max;
  };
  
  auto elem_select = [] (const vector<vector<node *>> &rows, size_t selection)
  {
    for(size_t i = 0; i < rows.size(); ++i)
    {
      const auto &c = rows[rows.size() - i - 1];
      const size_t elem_size = (i + 1);
      const size_t slot_size = elem_size * c.size();
      if(selection >= slot_size)
      {
        selection -= slot_size;
        continue;
      }
      
      return c[selection / elem_size];
    }
    
    return static_cast<node *>(nullptr);
  };
  
  auto prog1 = agents[0].program(), prog2 = agents[1].program();
  
  auto rows1 = prog1.rowize();
  auto rows2 = prog2.rowize();
  
  // Remove the root as a swap point.
  // This selection would be useless.
  rows1.erase(rows1.begin());
  rows2.erase(rows2.begin());
  
  node *const n1 = elem_select(rows1, rand() % find_max(rows1));
  node *const n2 = elem_select(rows2, rand() % find_max(rows2));
  */
  
  auto prog1 = agents[0].program(), prog2 = agents[1].program();
  node *const n1 = &prog1.find(rand() % prog1.num_nodes());
  node *const n2 = &prog2.find(rand() % prog2.num_nodes());
  
  swap(*n1, *n2);
  
  return std::vector<agent> { agent(prog1), agent(prog2) };
}
