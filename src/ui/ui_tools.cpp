#include <finch/ui/ui_tools.hpp>
#include <finch/ui/matrix2_view.hpp>

#include <QApplication>

using namespace finch;
using namespace finch::ui;

int finch::ui::show_matrix2(const matrix2<uint16_t> &mat)
{
  int argc = 0;
  char **argv = 0;
  QApplication app(argc, argv);
  matrix2_view view;
  view.resize(mat.columns() * 10, mat.rows() * 10);
  view.set_matrix(mat);
  view.show();
  view.raise();
  return app.exec();
}