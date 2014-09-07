#ifndef _FINCH_NODE_TYPE_HPP_
#define _FINCH_NODE_TYPE_HPP_

#include <stdint.h>
#include <string>

namespace finch
{
  class node_type
  {
  public:
    typedef uint32_t id_type;
    
    enum instruction_type
    {
      modifier = 0,
      conditional,
      other
    };
    
    node_type(const id_type id, const uint8_t num_children, const instruction_type itype, const std::string &name);
    node_type(const node_type &rhs);
    
    bool is_valid() const;
    bool is_terminal() const;
    
    instruction_type itype() const;
    
    id_type id() const;
    uint8_t num_children() const;
    
    const std::string &name() const;
    
    bool similar_to(const node_type &rhs) const;
    
    node_type &operator =(const node_type &rhs);
    
    bool operator <(const node_type &rhs) const;
    
  private:
    bool _valid;
    id_type _id;
    uint8_t _num_children;
    instruction_type _itype;
    std::string _name;
  };
  
  extern const node_type null_type;
}

#endif