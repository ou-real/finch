#ifndef _FINCH_UI_GRADIENT_VIEW_HPP_
#define _FINCH_UI_GRADIENT_VIEW_HPP_

#include <QWidget>
#include <QTimer>
#include <finch/matrix2.hpp>
#include <vector>

namespace Ui
{
  class gradient_view;
}

namespace finch
{
  namespace ui
  {
    class gradient_view : public QWidget
    {
      Q_OBJECT
    public:
      gradient_view(QWidget *const parent = 0);
      ~gradient_view();
      
      void set_gradients(const std::vector<matrix2<uint16_t> > &gradients);
      const std::vector<matrix2<uint16_t> > &gradients() const;
      
      void play();
      void pause();
      void fire();
      
      void set_current(const int current);
      
    private:
      void update();
      
      Ui::gradient_view *_ui;
      std::vector<matrix2<uint16_t> > _gradients;
      uint32_t _current;
      
      QTimer _fireman;
    };
  }
}

#endif