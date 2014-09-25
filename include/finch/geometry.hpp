#ifndef _FINCH_GEOMETRY_HPP_
#define _FINCH_GEOMETRY_HPP_

#include <ostream>
#include <cmath>

namespace finch
{
  template<typename T>
  class point2
  {
  public:
    typedef T type;
    
    point2()
    {
      _val[0] = T();
      _val[1] = T();
    }
    
    point2(const T &x, const T &y)
    {
      _val[0] = x;
      _val[1] = y;
    }
    
    const T &x() const
    {
      return _val[0];
    }
    
    void set_x(const T &x)
    {
      _val[0] = x;
    }
    
    const T &y() const
    {
      return _val[1];
    }
    
    void set_y(const T &y)
    {
      _val[1] = y;
    }
    
    const T &operator[](const size_t index) const
    {
      return _val[index];
    }
    
    T &operator[](const size_t index)
    {
      return _val[index];
    }
    
    point2<T> operator +(const point2<T> &rhs) const
    {
      return point2<T>(_val[0] + rhs._val[0], _val[1] + rhs._val[1]);
    }
    
    point2<T> operator -(const point2<T> &rhs) const
    {
      return point2<T>(_val[0] - rhs._val[0], _val[1] - rhs._val[1]);
    }
    
    bool operator ==(const point2<T> &rhs) const
    {
      return _val[0] == rhs._val[0] && _val[1] == rhs._val[1];
    }
    
    bool operator !=(const point2<T> &rhs) const
    {
      return _val[0] != rhs._val[0] || _val[1] != rhs._val[1];
    }
    
    template<typename R>
    R distance(const point2<T> &rhs) const
    {
      const R dx = _val[0] > rhs._val[0] ? _val[0] - rhs._val[0] : _val[0] - rhs._val[0];
      const R dy = _val[1] > rhs._val[1] ? _val[1] - rhs._val[1] : _val[1] - rhs._val[1];
      return sqrt(dx * dx + dy * dy);
    }
    
  private:
    T _val[2];
  };
  
  template<typename T>
  std::ostream &operator <<(std::ostream &out, const point2<T> &rhs)
  {
    return out << "(" << rhs.x() << ", " << rhs.y() << ")";
  }
  
  template<typename T>
  class rectangle
  {
  public:
    typedef T type;
    
    rectangle()
      : _origin()
      , _width(T())
      , _height(T())
    {
    }
    
    rectangle(point2<T> origin, const T &width, const T &height)
      : _origin(origin)
      , _width(width)
      , _height(height)
    {
    }
    
    rectangle(point2<T> top_left, point2<T> bottom_right)
      : _origin(top_left)
      , _width((top_left - bottom_right).x())
      , _height((top_left - bottom_right).y())
    {
    }
    
    const point2<T> &origin() const
    {
      return _origin;
    }
    
    const point2<T> &top_left() const
    {
      return _origin;
    }
    
    const point2<T> &bottom_right() const
    {
      return _origin + point2<T>(_width, _height);
    }
    
    const T &x() const
    {
      return _origin.x();
    }
    
    const T &y() const
    {
      return _origin.y();
    }
    
    const T &width() const
    {
      return _width;
    }
    
    const T &height() const
    {
      return _height;
    }
    
    T area() const
    {
      return _width * _height;
    }
    
    template<typename Predicate>
    void for_each(const Predicate &p)
    {
      for(T y = T(); y < _height; ++y) {
        for(T x = T(); x < _width; ++x) {
          p(_origin.x() + x, _origin.y() + y);
        }
      }
    }
    
    const point2<rectangle<T> > divide_vertical(const T &offset) const
    {
      if(offset >= _width) {
        return point2<rectangle<T> >(*this, rectangle<T>(_origin + point2<T>(_width, 0), 0, 0));
      }
      return point2<rectangle<T> >(rectangle<T>(_origin, offset, _height),
        rectangle<T>(_origin + point2<T>(offset, 0), _width - offset, _height));
    }
    
    const point2<rectangle<T> > divide_horizontal(const T &offset) const
    {
      if(offset >= _height) {
        return point2<rectangle<T> >(*this, rectangle<T>(_origin + point2<T>(0, _height), 0, 0));
      }
      return point2<rectangle<T> >(rectangle<T>(_origin, _width, offset),
        rectangle<T>(_origin + point2<T>(0, offset), _width, _height - offset));
    }
    
  private:
    point2<T> _origin;
    T _width;
    T _height;
  };
  
  template<typename T>
  std::ostream &operator <<(std::ostream &out, const rectangle<T> &rhs)
  {
    return out << "(" << rhs.origin() << ", w = " << rhs.width() << ", h = " << rhs.height() << ")";
  }
  
  enum cardinal_direction
  {
    east = 0,
    north,
    west,
    south
  };
}

#endif