#ifndef _FINCH_EXPRESSION_SIMPLIFIER_HPP_
#define _FINCH_EXPRESSION_SIMPLIFIER_HPP_

#include <finch/node_type_set.hpp>
#include <vector>
#include <finch/node.hpp>

namespace finch
{
  class expression_simplifier
  {
  public:
    expression_simplifier(const node_type_set &types);
    
    node simplify(const node &program) const;
    
  private:
    node visit(node n, std::vector<int16_t> seen) const;
    
    node_type_set _types;
  };
}

#endif