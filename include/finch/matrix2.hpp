#ifndef _FINCH_MATRIX2_HPP_
#define _FINCH_MATRIX2_HPP_

#include <string.h>
#include <stdint.h>
#include <ostream>
#include <istream>

namespace finch
{
  template<typename T>
  class matrix2
  {
  public:
    typedef uint16_t index;
    typedef T type;
    
    matrix2()
      : _rows(0)
      , _columns(0)
      , _data(0)
    {
    }
    
    matrix2(const index rows, const index columns, const T &fill = T())
      : _rows(rows)
      , _columns(columns)
      , _data(new T[_rows * _columns])
    {
      for(index i = 0; i < _rows * _columns; ++i) _data[i] = fill;
    }
    
    matrix2(const matrix2<T> &rhs)
      : _rows(rhs._rows)
      , _columns(rhs._columns)
      , _data(new T[_rows * _columns])
    {
      memcpy(_data, rhs._data, sizeof(T) * _rows * _columns);
    }
    
    ~matrix2()
    {
      delete[] _data;
    }
    
    index rows() const
    {
      return _rows;
    }
    
    index columns() const
    {
      return _columns;
    }
    
    T &at(const index row, const index column)
    {
      return _data[row * _columns + column];
    }
    
    const T &at(const index row, const index column) const
    {
      return _data[row * _columns + column];
    }
    
    T *ptr(const index row = 0)
    {
      return &_data[row * _columns];
    }
    
    const T *ptr(const index row = 0) const
    {
      return &_data[row * _columns];
    }
    
    void fill_border(const T &value)
    {
      for(index row = 0; row < _rows; ++row) at(row, 0) = value;
      for(index row = 0; row < _rows; ++row) at(row, _columns - 1) = value;
      for(index col = 0; col < _columns; ++col) at(0, col) = value;
      for(index col = 0; col < _columns; ++col) at(_rows - 1, col) = value;
    }
    
    bool index_of(const T &value, index &row, index &col) const
    {
      for(row = 0; row < _rows; ++row) {
        for(col = 0; col < _columns; ++col) {
          if(at(row, col) == value) return true;
        }
      }
      return false;
    }
    
    matrix2<T> &operator =(const matrix2<T> &rhs)
    {
      if(_rows * _columns != rhs._rows * rhs._columns) {
        delete[] _data;
        _data = new T[rhs._rows * rhs._columns];
      }
      
      _rows = rhs._rows;
      _columns = rhs._columns;
      memcpy(_data, rhs._data, sizeof(T) * _rows * _columns);
      return *this;
    }
    
  private:
    index _rows;
    index _columns;
    
    T *_data;
  };
  
  template<typename T>
  std::ostream &operator <<(std::ostream &out, const matrix2<T> &rhs)
  {
    for(typename matrix2<T>::index row = 0; row < rhs.rows(); ++row) {
      for(typename matrix2<T>::index column = 0; column < rhs.columns(); ++column) {
        out << rhs.at(row, column);
        if(column + 1 < rhs.columns()) out << ", ";
      }
      if(row + 1 < rhs.rows()) out << std::endl;
    }
    return out;
  }
  
  template<typename T>
  std::ostream &serialize(std::ostream &out, const matrix2<T> &rhs)
  {
    typename matrix2<T>::index w = rhs.columns();
    out.write(reinterpret_cast<const char *>(&w), sizeof(w));
    typename matrix2<T>::index h = rhs.rows();
    out.write(reinterpret_cast<const char *>(&h), sizeof(h));
    for(typename matrix2<T>::index row = 0; row < rhs.rows(); ++row) {
      for(typename matrix2<T>::index column = 0; column < rhs.columns(); ++column) {
        out.write(reinterpret_cast<const char *>(&rhs.at(row, column)), sizeof(T));
      }
    }
    return out;
  }
  
  template<typename T>
  std::istream &deserialize(std::istream &in, matrix2<T> &rhs)
  {
    typename matrix2<T>::index w = 0;
    in.read(reinterpret_cast<char *>(&w), sizeof(w));
    typename matrix2<T>::index h = 0;
    in.read(reinterpret_cast<char *>(&h), sizeof(h));
    
    rhs = matrix2<T>(h, w);
    for(typename matrix2<T>::index row = 0; row < rhs.rows(); ++row) {
      for(typename matrix2<T>::index column = 0; column < rhs.columns(); ++column) {
        in.read(reinterpret_cast<char *>(&rhs.at(row, column)), sizeof(T));
      }
    }
    return in;
  }
}


#endif