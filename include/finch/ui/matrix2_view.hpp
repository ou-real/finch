#ifndef _FINCH_MAIN_WINDOW_HPP_
#define _FINCH_MAIN_WINDOW_HPP_

#include <QWidget>

#include <finch/matrix2.hpp>

namespace finch
{
  namespace ui
  {
    class matrix2_view : public QWidget
    {
    public:
      matrix2_view(QWidget *const parent = 0);

      enum view_type
      {
        maze_view,
        gradient_view
      };
      
      void set_view(const view_type view);
      view_type view() const;

      void set_matrix(const matrix2<uint16_t> &mat);
      const matrix2<uint16_t> &matrix() const;
      
    protected:
      void paintEvent(QPaintEvent *event);

    private:
      void paint_maze(QPaintEvent *event);
      void paint_gradient(QPaintEvent *event);
      
      matrix2<uint16_t> _mat;
      view_type _view;
    };
  }
}

#endif