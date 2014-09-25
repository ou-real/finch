#include <finch/node_crossover_reproducer.hpp>

#include <iostream>

using namespace finch;

node_crossover_reproducer::node_crossover_reproducer()
  : reproducer(2)
{
}

std::vector<agent> node_crossover_reproducer::reproduce(const std::vector<agent> &agents) const
{
  node n1 = agents[0].program(), n2 = agents[1].program();
  node *swap[2] = {0, 0};
  
  const static uint32_t max_tries = 1;
  const uint32_t n1n = n1.num_nodes();
  const uint32_t n2n = n2.num_nodes();
  uint32_t i = 0;
  for(; i < max_tries; ++i) {
    swap[0] = &n1.find(rand() % n1n);
    swap[1] = &n2.find(rand() % n2n);
    if(swap[0]->type().similar_to(swap[1]->type())) break;
  }
  
  if(i < max_tries)
  {
    const node_type tmp = swap[0]->type();
    swap[0]->set_type(swap[1]->type());
    swap[1]->set_type(tmp);
  }
  
  return std::vector<agent> { agent(n1), agent(n2) };
}
