#include <finch/breeder.hpp>

using namespace finch;

breeder::breeder(const std::shared_ptr<reproducer> &implementation)
  : _implementation(implementation)
{
}

breeder::~breeder()
{
}

const std::shared_ptr<reproducer> &breeder::implementation() const
{
  return _implementation;
}

