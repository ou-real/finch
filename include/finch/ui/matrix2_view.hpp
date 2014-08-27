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

      void set_matrix(const matrix2<uint16_t> &mat);
      const matrix2<uint16_t> &matrix() const;
      
    protected:
      void paintEvent(QPaintEvent *event);

    private:
      matrix2<uint16_t> _mat;
    };
  }
}

#endif