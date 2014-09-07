#include "cpu_program_interpreter.hpp"
#include <finch/geometry.hpp>
#include <finch/program_state.hpp>

#include <stdio.h>
#include <iostream>

using namespace finch;
using namespace std;

static bool occurence_in_cone_east(uint16_t *const maze, const uint16_t rows, const uint16_t cols,
  uint16_t r, uint16_t c, const uint16_t val)
{
  const int16_t out = c;
  const int16_t in = r;
  const int16_t in_max = rows;
  const int16_t dir = 1;
  for(int16_t oi = out, k = 0; oi < cols; oi += dir, ++k) {
    const int16_t bound1 = (in - k < 0 ? 0 : in - k);
    const int16_t bound2 = (in + k > in_max - 1 ? in_max - 1 : in + k);
    for(int16_t ii = bound1; ii <= bound2; ++ii) {
      if(maze[ii * cols + oi] == 3) return true;
    }
  }
  return false;
}

static bool occurence_in_cone_north(uint16_t *const maze, const uint16_t rows, const uint16_t cols,
  uint16_t r, uint16_t c, const uint16_t val)
{
  const int16_t out = r;
  const int16_t in = c;
  const int16_t in_max = cols;
  const int16_t dir = -1;
  for(int16_t oi = out, k = 0; oi >= 0; oi += dir, ++k) {
    const int16_t bound1 = (in - k < 0 ? 0 : in - k);
    const int16_t bound2 = (in + k > in_max - 1 ? in_max - 1 : in + k);
    for(int16_t ii = bound1; ii <= bound2; ++ii) {
      if(maze[oi * cols + ii] == 3) return true;
    }
  }
  return false;
}

static bool occurence_in_cone_west(uint16_t *const maze, const uint16_t rows, const uint16_t cols,
  uint16_t r, uint16_t c, const uint16_t val)
{
  const int16_t out = c;
  const int16_t in = r;
  const int16_t in_max = rows;
  const int16_t dir = -1;
  for(int16_t oi = out, k = 0; oi >= 0; oi += dir, ++k) {
    const int16_t bound1 = (in - k < 0 ? 0 : in - k);
    const int16_t bound2 = (in + k > in_max - 1 ? in_max - 1 : in + k);
    for(int16_t ii = bound1; ii <= bound2; ++ii) {
      if(maze[ii * cols + oi] == 3) return true;
    }
  }
  
  return false;
}

static bool occurence_in_cone_south(uint16_t *const maze, const uint16_t rows, const uint16_t cols,
  uint16_t r, uint16_t c, const uint16_t val)
{
  const int16_t out = r;
  const int16_t in = c;
  const int16_t in_max = cols;
  const int16_t dir = 1;
  for(int16_t oi = out, k = 0; oi < rows; oi += dir, ++k) {
    const int16_t bound1 = (in - k < 0 ? 0 : in - k);
    const int16_t bound2 = (in + k > in_max - 1 ? in_max - 1 : in + k);
    for(int16_t ii = bound1; ii <= bound2; ++ii) {
      if(maze[oi * cols + ii] == 3) return true;
    }
  }
  
  return false;
}

static bool occurence_in_cone(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  const program_state state, const uint16_t val)
{
  switch(state.dir) {
    case east:  return occurence_in_cone_east (maze, rows, cols, state.row, state.col, val);
    case north: return occurence_in_cone_north(maze, rows, cols, state.row, state.col, val);
    case west:  return occurence_in_cone_west (maze, rows, cols, state.row, state.col, val);
    case south: return occurence_in_cone_south(maze, rows, cols, state.row, state.col, val);
  }
  
  // Should never be reached
  return false;
}

static bool occurence_ahead(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  const program_state state, const uint16_t val)
{
  switch(state.dir) {
    case east:  return state.col + 1 < cols && maze[state.row * cols + state.col + 1] == val;
    case north: return state.row && maze[(state.row - 1) * cols + state.col] == val;
    case west:  return state.col && maze[state.row * cols + state.col - 1] == val;
    case south: return state.row + 1 < rows && maze[(state.row + 1) * cols + state.col] == val;
  }
  
  // Should never be reached
  return false;
}

#define child_loc(i, t) (&op_loc[(t) + 1 + op_loc[(i) + 1]])

static const char *names[8] = {
  "hlt",
  "left",
  "right",
  "move",
  "if_wall_ahead",
  "if_goal_visible",
  "prog2",
  "debug_point"
};

void cpu_stepper_init(interpreter_state *const state)
{
  state->stack_head = 0;
  state->stack[0] = 0;
}

bool cpu_program_stepper(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *program, program_state &state, uint32_t &op_lim, interpreter_state *const istate)
{
  using namespace std;
  
  if(!op_lim) return true;
  
  istate->stack[istate->stack_head = 1] = program;
  while(op_lim > 0)
  {
    cout << "stack:" << endl;
    for(uint8_t i = istate->stack_head; i > 0; --i) cout << "\t" << istate->stack[i] << endl;
    
    uint32_t *const op_loc = istate->stack[istate->stack_head--];
    
    if(!op_loc) break;
    
    const uint32_t op = *op_loc;
    
    switch(op) {
    // left
    case 1: --op_lim; state.dir = (cardinal_direction)((state.dir + 1) % 4); cout << "[] l" << endl; break;
      
    // right
    case 2: --op_lim; state.dir = (cardinal_direction)(state.dir == 0 ? 3 : state.dir - 1); cout << "[] r" << endl; break;
      
    // move
    case 3: cout << "[] m" << endl; 
      --op_lim;
      if(occurence_ahead(maze, rows, cols, state, 1)) break;
      
      if(state.dir == north && state.row) --state.row;
      else if(state.dir == south && state.row + 1 < rows) ++state.row;
      else if(state.dir == west && state.col) --state.col;
      else if(state.dir == east && state.col + 1 < cols) ++state.col;

      break;
      
    // if_wall_ahead
    case 4: cout << "[] w" << endl;
      istate->stack[++istate->stack_head] = child_loc(occurence_ahead(maze, rows, cols, state, 1) ? 0 : 1, 2);
      break;
      
    // if_goal_visible
    case 5: cout << "[] g" << endl;
      istate->stack[++istate->stack_head] = child_loc(occurence_in_cone(maze, rows, cols, state, 2) ? 0 : 1, 2);
      
      break;
      
    // prog2
    case 6: cout << "[] p" << endl;
      istate->stack[++istate->stack_head] = child_loc(1, 2);
      istate->stack[++istate->stack_head] = child_loc(0, 2);
      break;
      
    case 7:
      --op_lim;
      printf("Reached debug point at %ld\n", op_loc - program);
      break;
      
    default:
      printf("--- UNKNOWN INSTRUCTION %u ---\n", op);
      return true;
    }
  }
  
  cout << "<" << state.row << ", " << state.col << "> " << "dir: " << state.dir << endl;
  
  return !op_lim;
}


void cpu_program_interpreter(uint16_t *const maze, const uint32_t rows, const uint32_t cols,
  uint32_t *offsets, uint32_t *programs, uint32_t program_index, program_state state, uint32_t op_lim, program_state *res)
{
  using namespace std;
  
  uint32_t *const our_program = programs + offsets[program_index];
  uint32_t *stack[255];
  uint8_t stack_head = 0;
  
  uint32_t ops_executed = 0;
  
  stack[0] = 0;
  while(op_lim) {
    if(stack_head == 255)
    {
      cout << "STACK OVERFLOW" << endl;
      return;
    }
    if(state.row >= rows)
    {
      cout << "HARD ERROR. ROWS EXCEEDED." << endl;
      return;
    }
    if(state.col >= cols)
    {
      cout << "HARD ERROR. COLS EXCEEDED." << endl;
      return;
    }
    uint32_t *const op_loc = stack[stack_head--];
    if(!op_loc) {
      stack[stack_head = 1] = our_program;
      continue;
    }
    const uint32_t op = *op_loc;
    // printf("%s\n", names[op]);
    
    switch(op) {
    // left
    case 1: --op_lim; state.dir = (cardinal_direction)((state.dir + 1) % 4); break;
      
    // right
    case 2: --op_lim; state.dir = (cardinal_direction)(state.dir == 0 ? 3 : state.dir - 1); break;
      
    // move
    case 3:
      --op_lim;
      if(occurence_ahead(maze, rows, cols, state, 1)) break;
      
      if(state.dir == north && state.row) --state.row;
      else if(state.dir == south && state.row + 1 < rows) ++state.row;
      else if(state.dir == west && state.col) --state.col;
      else if(state.dir == east && state.col + 1 < cols) ++state.col;
      break;
      
    // if_wall_ahead
    case 4:
      stack[++stack_head] = child_loc(occurence_ahead(maze, rows, cols, state, 1) ? 0 : 1, 2);
      break;
      
    // if_goal_visible
    case 5:
      stack[++stack_head] = child_loc(occurence_in_cone(maze, rows, cols, state, 2) ? 0 : 1, 2);
      break;
      
    // prog2
    case 6:
      stack[++stack_head] = child_loc(1, 2);
      stack[++stack_head] = child_loc(0, 2);
      break;
      
    case 7:
      --op_lim;
      printf("Reached debug point at %ld\n", op_loc - our_program);
      break;
      
    default:
      printf("--- UNKNOWN INSTRUCTION %u ---\n", op);
      return;
    }
    
    ++ops_executed;
  }
  
  res[program_index] = state;
}


