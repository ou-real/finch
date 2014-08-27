#include <finch/program_node_types.hpp>

using namespace finch;

const node_type types::left(1, 0, "left");
const node_type types::right(2, 0, "right");
const node_type types::move(3, 0, "move");

const node_type types::if_wall_ahead(4, 2, "if_wall_ahead");
const node_type types::if_goal_visible(5, 2, "if_goal_visible");
const node_type types::prog2(6, 2, "prog2");
const node_type types::debug_point(7, 0, "debug_point");

using namespace finch::types;

const node_type_set finch::program_types_set {
  left, right, move,
  if_wall_ahead, if_goal_visible, prog2
};