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
  
  double terminalProb = 0.4;
  double nonterminalProb = 0.6;
  const double totalProb = terminalProb + nonterminalProb;
  const uint32_t scaler = 1000;
  const uint32_t selection = rand() % static_cast<uint32_t>(totalProb * scaler);
  
  node *n1 = 0;
  node *n2 = 0;
  
  auto prog1 = agents[0].program(), prog2 = agents[1].program();
  if(selection <= terminalProb * scaler)
  {
    n1 = &prog1.find_terminal(rand() % prog1.terminals());
    n2 = &prog2.find_terminal(rand() % prog2.terminals());
  }
  else
  {
    n1 = &prog1.find_nonterminal(rand() % prog1.nonterminals());
    n2 = &prog2.find_nonterminal(rand() % prog2.nonterminals());
  }
  
  swap(*n1, *n2);
  
  return vector<agent> { agent(prog1), agent(prog2) };
}
