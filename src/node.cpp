#include <finch/node.hpp>
#include <sstream>

using namespace finch;

node finch::null_node(node_type(0, 0, node_type::other, ""));

node::node(const node_type &type)
  : _type(type)
{
  for(std::vector<node>::size_type i = 0; i < _type.num_children(); ++i) {
    _children.push_back(null_type);
  }
}

node::node(const node &rhs)
  : _type(rhs._type)
  , _children(rhs._children)
{
}

bool node::is_valid() const
{
  return _type.is_valid();
}

node &node::operator[](const size_t index)
{
  return _children[index];
}

const node &node::operator[](const size_t index) const
{
  return _children[index];
}

void node::set_type(const node_type &type)
{
  _type = type;
}

const node_type &node::type() const
{
  return _type;
}

node &node::find(const std::vector<node>::size_type i)
{
  std::vector<node>::size_type iter = i;
  return _find(iter);
}

const node &node::cfind(const std::vector<node>::size_type i) const
{
  std::vector<node>::size_type iter = i;
  return _cfind(iter);
}

std::vector<node>::size_type node::num_nodes() const
{
  std::vector<node>::size_type ret = 1;
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) ret += (*it).num_nodes();
  return ret;
}

std::vector<node>::size_type node::depth() const
{
  std::vector<node>::size_type ret = 0;
  for(const auto &c : _children) ret = std::max(ret, c.depth());
  return ret + 1;
}

std::vector<node_type::id_type>::size_type node::write_size() const
{
  std::vector<node_type::id_type>::size_type ret = 1 + _children.size();
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) ret += (*it).write_size();
  return ret;
}

void node::write(std::vector<node_type::id_type> &backing) const
{
  backing.push_back(_type.id());
  
  std::vector<node>::const_iterator it = _children.begin();
  for(uint32_t off = 0; it != _children.end(); ++it) {
    backing.push_back(off);
    off += (*it).write_size();
  }
  
  for(it = _children.begin(); it != _children.end(); ++it) (*it).write(backing);
}

std::ostream &node::write_dot(std::ostream &out) const
{
  using namespace std;
  uint32_t it = 0;
  out << "digraph {" << endl;
  return write_dot(out, it) << "}";
}


std::string node::sexpr() const
{
  std::stringstream s;
  s << "(" << _type.name();
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    s << " " << (*it).sexpr();
  }
  s << ")";
  
  return s.str();
}

std::vector<std::vector<node *> > node::rowize()
{
  using namespace std;
  
  vector<vector<node *> > ret;
  for(auto &child : _children)
  {
    const auto child_rows = child.rowize();
    auto rit = ret.begin();
    for(auto it = child_rows.begin();
      it != child_rows.end(); ++it, ++rit)
    {
      if(rit == ret.end())
      {
        ret.push_back(vector<node *> {});
        rit = ret.end() - 1;
      }
      rit->insert(rit->end(), it->begin(), it->end());
    }
  }
  vector<node *> this_row { this };
  ret.insert(ret.begin(), &this_row, &this_row + 1);
  return ret;
}

std::vector<std::vector<const node *> > node::crowize() const
{
  using namespace std;
  
  vector<vector<const node *> > ret;
  for(const auto &child : _children)
  {
    const auto child_rows = child.crowize();
    auto rit = ret.begin();
    for(auto it = child_rows.begin();
      it != child_rows.end(); ++it, ++rit)
    {
      if(rit == ret.end())
      {
        ret.push_back(vector<const node *> {});
        rit = ret.end() - 1;
      }
      rit->insert(rit->end(), it->begin(), it->end());
    }
  }
  vector<const node *> this_row { this };
  ret.insert(ret.begin(), &this_row, &this_row + 1);
  return ret;
}

uint32_t node::terminals() const
{
  if(_children.empty()) return 1;
  uint32_t ret = 0;
  for(const auto &child : _children) ret += child.terminals();
  return ret;
}

node &node::find_terminal(const std::vector<node>::size_type i)
{
  std::vector<node>::size_type iter = i;
  return _find_terminal(iter);
}

const node &node::cfind_terminal(const std::vector<node>::size_type i) const
{
  std::vector<node>::size_type iter = i;
  return _cfind_terminal(iter);
}

uint32_t node::nonterminals() const
{
  if(_children.empty()) return 0;
  uint32_t ret = 1;
  for(const auto &child : _children) ret += child.nonterminals();
  return ret;
}

node &node::find_nonterminal(const std::vector<node>::size_type i)
{
  std::vector<node>::size_type iter = i;
  return _find_nonterminal(iter);
}

const node &node::cfind_nonterminal(const std::vector<node>::size_type i) const
{
  std::vector<node>::size_type iter = i;
  return _cfind_nonterminal(iter);
}

node &node::operator =(const node &rhs)
{
  _type = rhs._type;
  _children = rhs._children;
  return *this;
}

std::ostream &node::write_dot(std::ostream &out, uint32_t &it, const uint32_t parent) const
{
  using namespace std;
  const string our_name = _type.name();
  const uint32_t our_it = ++it;
  out << "n" << our_it << " [label=\"" << our_name << "\"];" << endl;
  if(parent) out << "n" << parent << " -> n" << our_it << ";" << endl;
  for(auto child : _children) child.write_dot(out, it, our_it);
  return out;
}

node &node::_find_terminal(std::vector<node>::size_type &i)
{
  if(_children.empty())
  {
    if(!i) return *this;
    --i;
  }
  std::vector<node>::iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    node &ret = it->_find_terminal(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}

const node &node::_cfind_terminal(std::vector<node>::size_type &i) const
{
  if(_children.empty())
  {
    if(!i) return *this;
    --i;
  }
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    const node &ret = it->_cfind_terminal(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}

node &node::_find_nonterminal(std::vector<node>::size_type &i)
{
  if(!i) return *this;
  --i;
  std::vector<node>::iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    if(it->_children.empty()) continue;
    node &ret = it->_find_nonterminal(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}

const node &node::_cfind_nonterminal(std::vector<node>::size_type &i) const
{
  if(!i) return *this;
  --i;
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    if(it->_children.empty()) continue;
    const node &ret = it->_cfind_nonterminal(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}

node &node::_find(std::vector<node>::size_type &i)
{
  if(!i) return *this;
  --i;
  std::vector<node>::iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    node &ret = it->_find(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}

const node &node::_cfind(std::vector<node>::size_type &i) const
{
  if(!i) return *this;
  --i;
  std::vector<node>::const_iterator it = _children.begin();
  for(; it != _children.end(); ++it) {
    const node &ret = it->_cfind(i);
    if(ret.is_valid()) return ret;
  }
  return null_node;
}
