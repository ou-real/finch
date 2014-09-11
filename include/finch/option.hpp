#ifndef _FINCH_OPTION_HPP_
#define _FINCH_OPTION_HPP_

#include <new>

namespace finch
{
  template<typename T>
  class option
  {
  public:
    template<typename F>
    option(const F &t)
      : _empty(false)
    {
      new (_t) T(t);
    }
    
    option()
      : _empty(true)
    {
    }
    
    option(const option &rhs)
      : _empty(rhs._empty)
    {
      if(_empty) return;
      new (_t) T(rhs.get());
    }
    
    ~option()
    {
      if(!_empty) reinterpret_cast<T *>(_t)->~T();
    }
    
    inline bool some() const
    {
      return !_empty;
    }
    
    inline bool none() const
    {
      return _empty;
    }
    
    inline T &get()
    {
      return *reinterpret_cast<T *>(_t);
    }
    
    inline const T &get() const
    {
      return *reinterpret_cast<const T *>(_t);
    }
    
  private:
    bool _empty;
    unsigned char _t[sizeof(T)];
  };
}

#endif