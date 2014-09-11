#include <finch/output_manager.hpp>

using namespace finch;

output_manager::output_manager(const std::string &toplevel_path)
  : _toplevel_path(toplevel_path)
{
}

std::string output_manager::path(const std::string &name) const
{
  return _toplevel_path + "/" + name;
}