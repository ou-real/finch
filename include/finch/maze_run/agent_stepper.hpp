#ifndef _FINCH_UI_AGENT_STEPPER_HPP_
#define _FINCH_UI_AGENT_STEPPER_HPP_

#include <QWidget>
#include <finch/matrix2.hpp>

namespace Ui
{
  class agent_stepper;
}

namespace finch
{
  class cpu_stepper;
  
  namespace ui
  {
    class agent_stepper : public QWidget
    {
      Q_OBJECT
    public:
      agent_stepper(QWidget *const parent = 0);
      ~agent_stepper();
      
      void set_stepper(cpu_stepper *const stepper);
      cpu_stepper *stepper() const;
      
      void reset();
      void step();
      
    private:
      void update();
      
      Ui::agent_stepper *_ui;
      cpu_stepper *_stepper;
      uint32_t _op_lim;
      matrix2<uint16_t> _maze;
      uint32_t _last_pos_r;
      uint32_t _last_pos_c;
      uint16_t _last_val;
    };
  }
}

#endif