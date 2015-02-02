#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <QApplication>
#include <finch/gradient_view/gradient_view.hpp>

#include "ui_gradient_view.h"

using namespace std;
using namespace finch;
using namespace finch::ui;

gradient_view::gradient_view(QWidget *const parent, bool save)
  : QWidget(parent)
  , _ui(new Ui::gradient_view)
  , _current(0)
  , _save(save)
{
  _ui->setupUi(this);
  
  _ui->matrix->set_view(matrix2_view::gradient_view);
  
  connect(_ui->play, &QPushButton::clicked, this, &gradient_view::play);
  connect(_ui->pause, &QPushButton::clicked, this, &gradient_view::pause);
  connect(&_fireman, &QTimer::timeout, this, &gradient_view::fire);
  
  connect(_ui->playhead, &QAbstractSlider::sliderMoved, this, &gradient_view::set_current);
}

gradient_view::~gradient_view()
{
  delete _ui;
}

void gradient_view::set_gradients(const vector<matrix2<uint16_t> > &gradients)
{
  _gradients = gradients;
  _current = 0;
  update();
}

const vector<matrix2<uint16_t> > &gradient_view::gradients() const
{
  return _gradients;
}

void gradient_view::play()
{
  _fireman.start(150);
}

void gradient_view::pause()
{
  _fireman.stop();
}

void gradient_view::fire()
{
  if(!_gradients.empty()) _current = std::min(static_cast<unsigned>(_current + 1), static_cast<unsigned>(_gradients.size() - 1));
  update();
}

void gradient_view::set_current(const int current)
{
  _fireman.stop();
  _current = current;
  update();
}

void gradient_view::update()
{
  _ui->playhead->setRange(0, _gradients.size());
  _ui->playhead->setSliderPosition(_current);
  if(_gradients.empty()) return;
  _ui->matrix->set_matrix(_gradients[_current]);
  QPixmap::grabWidget(_ui->matrix).save(QString("grad-%1").arg(_current), "PNG");
}

void usage(const char *const path)
{
  std::cout << path << " [file]+" << std::endl;
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    usage(argv[0]);
    return 1;
  }
  
  vector<matrix2<uint16_t> > grads;
  
  for(int i = 1; i < argc; ++i)
  {
    std::ifstream in(argv[i]);
    if(!in.is_open()) {
      std::cerr << "Failed to open \"" << argv[1] << "\" for reading!" << std::endl;
      return 1;
    }
  
    matrix2<uint16_t> grad;
    deserialize(in, grad);
  
    in.close();
    
    grads.push_back(grad);
  }
  
  QApplication app(argc, argv);
  gradient_view view(0, true);
  view.set_gradients(grads);
  view.show();
  view.raise();
  return app.exec();
}