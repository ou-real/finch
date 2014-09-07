#include <finch/program_node_types.hpp>

using namespace finch;

const node_type types::left(1, 0, node_type::modifier, "left");
const node_type types::right(2, 0, node_type::modifier, "right");
const node_type types::move(3, 0, node_type::modifier, "move");

const node_type types::if_wall_ahead(4, 2, node_type::conditional, "if_wall_ahead");
const node_type types::if_goal_visible(5, 2, node_type::conditional, "if_goal_visible");
const node_type types::prog2(6, 2, node_type::other, "prog2");
const node_type types::debug_point(7, 0, node_type::other, "debug_point");

using namespace finch::types;

const node_type_set finch::program_types_set {
  left, right, move,
  if_wall_ahead, if_goal_visible, prog2
};