#ifndef _FINCH_NODE_HPP_
#define _FINCH_NODE_HPP_

#include "node_type.hpp"
#include <vector>
#include <string>
#include <ostream>

namespace finch
{
  class node
  {
  public:
    node(const node_type &type);
    node(const node &rhs);
    
    bool is_valid() const;
    
    node &operator[](const size_t index);
    const node &operator[](const size_t index) const;
    
    void set_type(const node_type &type);
    const node_type &type() const;
    
    node &find(const std::vector<node>::size_type i);
    const node &cfind(const std::vector<node>::size_type i) const;
    
    std::vector<node>::size_type num_nodes() const;
    
    std::vector<node_type::id_type>::size_type write_size() const;
    void write(std::vector<node_type::id_type> &backing) const;
    
    std::ostream &write_dot(std::ostream &out) const;
    
    std::string sexpr() const;
    
    std::vector<std::vector<node *> > rowize();
    std::vector<std::vector<const node *> > crowize() const;
    
    uint32_t terminals() const;
    
    node &find_terminal(const std::vector<node>::size_type i);
    const node &cfind_terminal(const std::vector<node>::size_type i) const;
    
    uint32_t nonterminals() const;
    
    node &find_nonterminal(const std::vector<node>::size_type i);
    const node &cfind_nonterminal(const std::vector<node>::size_type i) const;
    
    node &operator =(const node &rhs);
    
  private:
    std::ostream &write_dot(std::ostream &out, uint32_t &it, const uint32_t parent = 0) const;
    
    node &_find(std::vector<node>::size_type &i);
    const node &_cfind(std::vector<node>::size_type &i) const;
    
    node &_find_terminal(std::vector<node>::size_type &i);
    const node &_cfind_terminal(std::vector<node>::size_type &i) const;
    
    node &_find_nonterminal(std::vector<node>::size_type &i);
    const node &_cfind_nonterminal(std::vector<node>::size_type &i) const;
    
    node_type _type;
    std::vector<node> _children;
  };
  
  extern node null_node;
}

#endif