#include <finch/node_type.hpp>

using namespace finch;

const node_type finch::null_type(0, 0, "");

node_type::node_type(const node_type::id_type id, const uint8_t num_children, const std::string &name)
  : _id(id)
  , _num_children(num_children)
  , _name(name)
{
}

node_type::node_type(const node_type &rhs)
  : _id(rhs._id)
  , _num_children(rhs._num_children)
  , _name(rhs._name)
{
}

bool node_type::is_valid() const
{
  return _id;
}

bool node_type::is_terminal() const
{
  return !_num_children;
}

node_type::id_type node_type::id() const
{
  return _id;
}

uint8_t node_type::num_children() const
{
  return _num_children;
}

const std::string &node_type::name() const
{
  return _name;
}

bool node_type::similar_to(const node_type &rhs) const
{
  return _num_children == rhs._num_children;
}

node_type &node_type::operator =(const node_type &rhs)
{
  _id = rhs._id;
  _num_children = rhs._num_children;
  _name = rhs._name;
  
  return *this;
}

bool node_type::operator <(const node_type &rhs) const
{
  return _id < rhs._id;
}
