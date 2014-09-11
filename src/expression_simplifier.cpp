#include <finch/expression_simplifier.hpp>

#include <iostream>
#include <fstream>

using namespace finch;

expression_simplifier::expression_simplifier(const node_type_set &types)
  : _types(types)
{
}

node expression_simplifier::simplify(const node &program) const
{
  using namespace std;
  node_type::id_type max_id = 0;
  for(const auto &type : _types) max_id = max(max_id, type.id());
  node ret = visit(program, vector<int16_t>(max_id + 1, -1));
  return ret;
}

node expression_simplifier::visit(node n, std::vector<int16_t> seen) const
{
  using namespace std;
  
  if(seen[n.type().id()] >= 0)
  {
    const uint8_t dir = seen[n.type().id()];
    return visit(n[dir], seen);
  }
  
  for(uint8_t i = 0; i < n.type().num_children(); ++i)
  {
    if(n.type().itype() == node_type::conditional)
    {
      seen[n.type().id()] = i;
    }
    else if(n.type().itype() == node_type::other)
    {
      // All bets are off if we see other
      seen.assign(seen.size(), -1);
    }
    
    n[i] = visit(n[i], seen);
  }
  
  return n;
}