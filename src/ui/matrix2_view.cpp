#include <finch/ui/matrix2_view.hpp>

#include <QPainter>

using namespace finch;
using namespace finch::ui;

matrix2_view::matrix2_view(QWidget *const parent)
  : QWidget(parent)
  , _mat(0, 0)
{
}

void matrix2_view::set_matrix(const matrix2<uint16_t> &mat)
{
  _mat = mat;
}

const matrix2<uint16_t> &matrix2_view::matrix() const
{
  return _mat;
}

void matrix2_view::paintEvent(QPaintEvent *event)
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
      const uint8_t v = _mat.at(r, c);
      QColor co;
      switch(v) {
        case 1: co = QColor(0, 0, 0); break;
        case 2: co = QColor(255, 0, 0); break;
        case 3: co = QColor(0, 255, 0); break;
        case 4: co = QColor(0, 0, 255); break;
        default: continue;
      }
      
      p.fillRect(xOff + blockWidth * c, yOff + blockHeight * r, blockWidth + 1.0f, blockHeight + 1.0f, co);
    }
  }
}