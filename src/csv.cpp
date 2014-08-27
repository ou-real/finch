#include <finch/csv.hpp>

using namespace finch;

csv::column_handle csv::append_column(const std::string &name)
{
  using namespace std;
  
  _table.push_back(make_tuple(name, vector<string>()));
  return _table.size() - 1;
}

void csv::append_data(const column_handle handle, const std::string &data)
{
  using namespace std;
  get<1>(_table[handle]).push_back(data);
}

void csv::clear()
{
  _table.clear();
}

std::ostream &csv::write(std::ostream &out) const
{
  using namespace std;
  uint32_t max_size = 0;
  for(auto column : _table) {
    max_size = max(max_size, static_cast<uint32_t>(get<1>(column).size()));
  }
  
  {
    bool first = true;
    for(auto column : _table) {
      if(!first) out << ", ";
      else first = false;
      out << get<0>(column);
    }
    out << endl;
  }
  
  for(uint32_t i = 0; i < max_size; ++i) {
    bool first = true;
    for(auto column : _table) {
      if(!first) out << ", ";
      else first = false;
      auto data_vec = get<1>(column);
      if(i >= data_vec.size()) continue;
      out << data_vec[i];
    }
    out << endl;
  }
  
  return out;
}