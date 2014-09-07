#include <finch/ui/ui_tools.hpp>
#include <finch/ui/matrix2_view.hpp>

#include <QApplication>
#include <iostream>

using namespace finch;
using namespace finch::ui;

int finch::ui::show_matrix2(const matrix2<uint16_t> &mat, const matrix2_view_type view_type)
{
  int argc = 0;
  char **argv = 0;
  QApplication app(argc, argv);
  matrix2_view view;
  switch(view_type)
  {
    case ui::maze_view: view.set_view(matrix2_view::maze_view); break;
    case ui::gradient_view: view.set_view(matrix2_view::gradient_view); break;
  }
  view.resize(mat.columns() * 10, mat.rows() * 10);
  view.set_matrix(mat);
  view.show();
  view.raise();
  return app.exec();
}