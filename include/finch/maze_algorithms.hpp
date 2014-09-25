#ifndef _FINCH_MAZE_ALGORITHMS_HPP_
#define _FINCH_MAZE_ALGORITHMS_HPP_

#include "matrix2.hpp"
#include "geometry.hpp"
#include <cstdlib>
#include <deque>
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>

#warning remove me
#include <iostream>

#include <tuple>

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
  template<> struct hash<finch::point2<typename finch::matrix2<uint16_t>::index>>
  {
    inline size_t operator()(const finch::point2<typename finch::matrix2<uint16_t>::index> & v) const
    {
      size_t seed = 0;
      ::hash_combine(seed, v.x());
      ::hash_combine(seed, v.y());
      return seed;
    }
  };
}

namespace finch
{
  template<typename T>
  class fill_predicate
  {
  public:
    fill_predicate(matrix2<T> &mat, const T &value)
      : _mat(mat)
      , _value(value)
    {
    }
    
    void operator()(typename matrix2<T>::index x, typename matrix2<T>::index y) const
    {
      _mat.at(y, x) = _value;
    }
    
  private:
    matrix2<T> &_mat;
    T _value;
  };
  
  template<typename T>
  void subdivide(matrix2<T> &mat, const T &empty_value, const T &occupied_value, const typename matrix2<T>::index cutoff, const bool b)
  {
    using namespace std;
    
    typedef point2<typename matrix2<T>::index> mat_point;
    typedef rectangle<typename matrix2<T>::index> mat_rect;
    typedef tuple<mat_rect, bool> tuple;
    
    const static uint8_t max_tries = 200;
    
    deque<tuple> _regions;
    _regions.push_back(tuple(mat_rect(mat_point(b ? 1 : 0, b ? 1 : 0), mat.columns() - (b ? 2 : 0), mat.rows() - (b ? 2 : 0)), rand() % 2));
    while(!_regions.empty())
    {
      const tuple t = _regions.front();
      _regions.pop_front();
      
      const mat_rect c = std::get<0>(t);
      
      if(c.area() <= cutoff) continue;
      if((get<1>(t) && c.width() < 3) || (!get<1>(t) && c.height() < 3)) continue;
      
      point2<mat_rect> res;
      point2<mat_rect> r;
      
      mat_point hole;
      typename matrix2<T>::index loc = 0;
      
      if(!get<1>(t)) {
        uint8_t i = 0;
        for(; i < max_tries; ++i) {
          loc = rand() % (c.width() - 2) + 1;
          if(c.y() && mat.at(c.y() - 1, c.x() + loc) == empty_value) continue;
          if(c.y() + c.height() < mat.rows()  && mat.at(c.y() + c.height(), c.x() + loc) == empty_value) continue;
          break;
        }
        if(i >= max_tries) continue;
        res = c.divide_vertical(loc + 1);
        r = res[0].divide_vertical(res[0].width() - 1);
        hole = mat_point(r[1].x(), r[1].y() + rand() % r[1].height());
      } else {
        uint8_t i = 0;
        for(; i < max_tries; ++i) {
          loc = rand() % (c.height() - 2) + 1;
          if(c.x() && mat.at(c.y() + loc, c.x() - 1) == empty_value) continue;
          if(c.x() + c.width() < mat.columns() && mat.at(c.y() + loc, c.x() + c.width()) == empty_value) continue;
          break;
        }
        if(i >= max_tries) continue;
        res = c.divide_horizontal(loc + 1);
        r = res[0].divide_horizontal(res[0].height() - 1);
        hole = mat_point(r[1].x() + rand() % r[1].width(), r[1].y());
      }
      
      res[0] = r[0];
      r[1].for_each(fill_predicate<T>(mat, occupied_value));
      mat.at(hole.y(), hole.x()) = empty_value;
      
      _regions.push_back(tuple(res[0], !get<1>(t)));
      _regions.push_back(tuple(res[1], !get<1>(t)));
    }
  }
  
  template<typename T>
  point2<typename matrix2<T>::index> random_location_satisfying(const matrix2<T> &mat, const T &value)
  {
    typedef typename matrix2<T>::index ind;
    
    std::vector<point2<ind> > satisfy;
    for(ind r = 0; r < mat.rows(); ++r) {
      for(ind c = 0; c < mat.columns(); ++c) {
        if(mat.at(r, c) == value) satisfy.push_back(point2<ind>(c, r));
      }
    }
    
    if(satisfy.empty()) {
      // ERROR: Throw exception??
    }
    
    return satisfy[rand() % satisfy.size()];
  }
  
  template<typename T>
  std::vector<point2<typename matrix2<T>::index> > adjacent_satisfying(const matrix2<T> &mat, const point2<typename matrix2<T>::index> &p, const T &value)
  {
    typedef point2<typename matrix2<T>::index> pt_type;
    typedef std::vector<pt_type> ret_type;
    if(!mat.rows() || !mat.columns()) return ret_type();
    
    ret_type ret;
    if(p.x() && mat.at(p.y(), p.x() - 1) == value)
      ret.push_back(pt_type(p.x() - 1, p.y()));
    
    if(p.x() + 1 < mat.columns() && mat.at(p.y(), p.x() + 1) == value)
      ret.push_back(pt_type(p.x() + 1, p.y()));
    
    if(p.y() && mat.at(p.y() - 1, p.x()) == value)
      ret.push_back(pt_type(p.x(), p.y() - 1));
    
    if(p.y() + 1 < mat.rows() && mat.at(p.y() + 1, p.x()) == value)
      ret.push_back(pt_type(p.x(), p.y() + 1));
    
    return ret;
  }
  
  
  
  template<typename T>
  std::vector<point2<typename matrix2<T>::index> > path(const matrix2<T> &mat,
    const point2<typename matrix2<T>::index> &start,
    const point2<typename matrix2<T>::index> &end)
  {
    using namespace std;
    typedef point2<typename matrix2<T>::index> pt_type;
    
    deque<pt_type> queue;
    vector<pt_type> closed;
    unordered_map<pt_type, pt_type> from;
    unordered_map<pt_type, double> g_score;
    unordered_map<pt_type, double> f_score;
    g_score[start] = 0.0;
    f_score[start] = g_score[start] + start. template distance<double>(end);
    queue.push_back(start);
    while(!queue.empty()) {
      sort(queue.begin(), queue.end(), [&] (const pt_type &l, const pt_type &r) {
        return f_score[l] < f_score[r];
      });
      const pt_type p = queue.front();
      queue.pop_front();
      
      if(p == end) {
        vector<pt_type> ret;
        pt_type current = p;
        while(from.find(current) != from.end()) {
          ret.push_back(current);
          current = from[current];
        }
        return ret;
      }
      
      closed.push_back(p);
      vector<pt_type> matches = adjacent_satisfying<T>(mat, p, 0);

      for(const auto &adj : matches) {
        if(find(closed.begin(), closed.end(), adj) != closed.end()) continue;
        const double g = g_score[p] + p. template distance<double>(adj);
        if(find(queue.begin(), queue.end(), adj) == queue.end() || g < g_score[adj])
        {
          from[adj] = p;
          g_score[adj] = g;
          f_score[adj] = g_score[adj] + adj. template distance<double>(end);
          for(auto it = closed.begin(); it != closed.end();)
          {
            if(*it == adj)
            {
              it = closed.erase(it);
              continue;
            }
            ++it;
          }
          if(find(queue.begin(), queue.end(), adj) == queue.end()) queue.push_back(adj);
        }
      }
    }
    
    return vector<pt_type>();
  }
}

#endif