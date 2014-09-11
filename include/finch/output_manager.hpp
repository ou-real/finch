#ifndef _FINCH_OUTPUT_MANAGER_HPP_
#define _FINCH_OUTPUT_MANAGER_HPP_

#include <string>

namespace finch
{
  class output_manager
  {
  public:
    output_manager(const std::string &toplevel_path);
    std::string path(const std::string &name) const;
    
  private:
    std::string _toplevel_path;
  };
}

#endif