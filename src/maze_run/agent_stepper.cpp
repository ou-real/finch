#include <finch/maze_run/agent_stepper.hpp>
#include <finch/cpu/cpu_stepper.hpp>
#include "ui_agent_stepper.h"

using namespace finch;
using namespace finch::ui;

agent_stepper::agent_stepper(QWidget *const parent)
  : QWidget(parent)
  , _ui(new Ui::agent_stepper)
  , _stepper(0)
  , _last_pos_r(0)
  , _last_pos_c(0)
  , _last_val(0)
{
  _ui->setupUi(this);
  connect(_ui->reset, &QPushButton::clicked, this, &agent_stepper::reset);
  connect(_ui->step, &QPushButton::clicked, this, &agent_stepper::step);
  reset();
}

agent_stepper::~agent_stepper()
{
  delete _ui;
}

void agent_stepper::set_stepper(cpu_stepper *const stepper)
{
  _stepper = stepper;
  _maze = _stepper->maze();
  _ui->matrix->set_matrix(_maze);
}

cpu_stepper *agent_stepper::stepper() const
{
  return _stepper;
}

void agent_stepper::step()
{
  if(_stepper) _stepper->step(_op_lim);
  update();
}

void agent_stepper::reset()
{
  if(_stepper) _stepper->reset();
  _op_lim = 200;
  update();
}

void agent_stepper::update()
{
  if(!_stepper) return;
  if(_op_lim < 200)
  {
    _maze.at(_last_pos_r, _last_pos_c) = _last_val;
  }
  
  _last_pos_c = _stepper->state().col;
  _last_pos_r = _stepper->state().row;
  _last_val = _maze.at(_last_pos_r, _last_pos_c);
  matrix2<uint16_t> disp = _stepper->mod();
  disp.at(_last_pos_r, _last_pos_c) = 0xFFFF;
  _ui->matrix->set_matrix(disp);
}