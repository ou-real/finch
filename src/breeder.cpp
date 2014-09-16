#include <finch/breeder.hpp>

using namespace finch;

breeder::breeder(const experimental_parameters &exp_params)
  : _exp_params(exp_params)
{
  
}

breeder::~breeder()
{
  
}

const experimental_parameters &breeder::exp_params() const
{
  return _exp_params;
}