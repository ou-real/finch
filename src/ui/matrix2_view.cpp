#include <finch/ui/matrix2_view.hpp>

#include <QPainter>
#include <QDebug>

using namespace finch;
using namespace finch::ui;

matrix2_view::matrix2_view(QWidget *const parent)
  : QWidget(parent)
  , _mat(0, 0)
  , _view(matrix2_view::maze_view)
{
}

void matrix2_view::set_matrix(const matrix2<uint16_t> &mat)
{
  _mat = mat;
  update();
}

const matrix2<uint16_t> &matrix2_view::matrix() const
{
  return _mat;
}

void matrix2_view::set_view(const view_type view)
{
  if(_view == view) return;
  _view = view;
}

matrix2_view::view_type matrix2_view::view() const
{
  return _view;
}

void matrix2_view::paintEvent(QPaintEvent *event)
{
  switch(_view)
  {
    case matrix2_view::maze_view: paint_maze(event); break;
    case matrix2_view::gradient_view: paint_gradient(event); break;
  }
}

void matrix2_view::paint_maze(QPaintEvent *event)
{
  QPainter p(this);
  p.fillRect(0, 0, width() - 1, height() - 1, palette().window());
  
  if(!_mat.rows() || !_mat.columns()) return;
  
  const float size = qMin(width(), height());
  const float xOff = (width() - size) / 2.0f;
  const float yOff = (height() - size) / 2.0f;
  
  const float blockWidth = size / _mat.columns();
  const float blockHeight = size / _mat.rows();
  
  for(matrix2<uint16_t>::index r = 0; r < _mat.rows(); ++r) {
    for(matrix2<uint16_t>::index c = 0; c < _mat.columns(); ++c) {
      const uint8_t v = _mat.at(r, c);
      QColor co;
      switch(v) {
        case 0: co = QColor(255, 255, 255); break;
        default: continue;
      }
      
      p.fillRect(xOff + blockWidth * c, yOff + blockHeight * r, blockWidth + 1.0f, blockHeight + 1.0f, co);
    }
  }
  
  p.setPen(QPen(QColor(220, 220, 220, 200), 1));
  for(matrix2<uint16_t>::index c = 0; c <= _mat.columns(); ++c) {
    p.drawLine(xOff + blockWidth * c, yOff, xOff + blockWidth * c, yOff + size);
  }
  
  for(matrix2<uint16_t>::index r = 0; r <= _mat.rows(); ++r) {
    p.drawLine(xOff, yOff + blockHeight * r, xOff + size, yOff + blockHeight * r);
  }
  
  for(matrix2<uint16_t>::index r = 0; r < _mat.rows(); ++r) {
    for(matrix2<uint16_t>::index c = 0; c < _mat.columns(); ++c) {
      const uint16_t v = _mat.at(r, c);
      QColor co;
      switch(v) {
        case 1: co = QColor(0, 0, 0); break;
        case 2: co = QColor(255, 0, 0); break;
        case 3: co = QColor(0, 255, 0); break;
        case 4: co = QColor(0, 0, 255); break;
        case 0xAAAA: co = QColor(255, 0, 255); break;
        case 0xFFFF: co = QColor(0, 255, 255); break;
        default: continue;
      }
      
      p.fillRect(xOff + blockWidth * c, yOff + blockHeight * r, blockWidth + 1.0f, blockHeight + 1.0f, co);
    }
  }
}

void matrix2_view::paint_gradient(QPaintEvent *event)
{
  QPainter p(this);
  p.fillRect(0, 0, width() - 1, height() - 1, palette().window());
  
  if(!_mat.rows() || !_mat.columns()) return;
  
  const float size = qMin(width(), height());
  const float xOff = (width() - size) / 2.0f;
  const float yOff = (height() - size) / 2.0f;
  
  const float blockWidth = size / _mat.columns();
  const float blockHeight = size / _mat.rows();
  
  for(matrix2<uint16_t>::index r = 0; r < _mat.rows(); ++r) {
    for(matrix2<uint16_t>::index c = 0; c < _mat.columns(); ++c) {
      p.fillRect(xOff + blockWidth * c, yOff + blockHeight * r,
        blockWidth + 1.0f, blockHeight + 1.0f, QColor(255, 255, 255));
    }
  }
  
  p.setPen(QPen(QColor(220, 220, 220, 200), 1));
  for(matrix2<uint16_t>::index c = 0; c <= _mat.columns(); ++c) {
    p.drawLine(xOff + blockWidth * c, yOff, xOff + blockWidth * c, yOff + size);
  }
  
  for(matrix2<uint16_t>::index r = 0; r <= _mat.rows(); ++r) {
    p.drawLine(xOff, yOff + blockHeight * r, xOff + size, yOff + blockHeight * r);
  }
  
  for(matrix2<uint16_t>::index r = 0; r < _mat.rows(); ++r) {
    for(matrix2<uint16_t>::index c = 0; c < _mat.columns(); ++c) {
      const uint16_t v = _mat.at(r, c);
      const QColor co(v / 255, v / 255, v / 255);
      p.fillRect(xOff + blockWidth * c, yOff + blockHeight * r,
        blockWidth + 1.0f, blockHeight + 1.0f, co);
    }
  }
}