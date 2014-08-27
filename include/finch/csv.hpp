#ifndef _FINCH_CSV_HPP_
#define _FINCH_CSV_HPP_

#include <vector>
#include <tuple>
#include <string>
#include <ostream>

namespace finch
{
  class csv
  {
  public:
    typedef std::tuple<std::string, std::vector<std::string> > column_type;
    typedef std::vector<column_type> table_type;
    
    typedef table_type::size_type column_handle;
    
    column_handle append_column(const std::string &name);
    
    void append_data(const column_handle handle, const std::string &data);
    
    void clear();
    
    std::ostream &write(std::ostream &out) const;
    
  private:
    table_type _table;
  };
}

#endif