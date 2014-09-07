#ifndef _FINCH_UI_TOOLS_HPP_
#define _FINCH_UI_TOOLS_HPP_

#include <finch/matrix2.hpp>
#include <finch/agent.hpp>

namespace finch
{
  namespace ui
  {
    enum matrix2_view_type
    {
      maze_view,
      gradient_view
    };
    
    int show_matrix2(const matrix2<uint16_t> &mat, const matrix2_view_type view_type);
  }
}

#endif