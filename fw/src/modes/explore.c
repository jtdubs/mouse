#include "modes/explore.h"

#include <avr/interrupt.h>
#include <stddef.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "control/linear.h"
#include "control/plan.h"
#include "control/position.h"
#include "control/speed.h"
#include "control/walls.h"
#include "maze/maze.h"
#include "modes/remote.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"
#include "utils/assert.h"

typedef enum : uint8_t { NORTH, EAST, SOUTH, WEST, INVALID } orientation_t;

constexpr float ENTRY_OFFSET = 16.0;               // mm
constexpr float CELL_SIZE    = 180.0;              // mm
constexpr float CELL_SIZE_2  = (CELL_SIZE / 2.0);  // mm

static orientation_t explore_orientation;
static float         explore_cell_offset;
static bool          explore_stopped;

static maze_location_t explorer_path_stack[256];
static maze_location_t explorer_next_stack[256];
static uint8_t         explorer_path_top;
static uint8_t         explorer_next_top;

typedef enum : uint8_t {
  STACK_PATH,
  STACK_NEXT,
} stack_id_t;

#pragma pack(push, 1)
typedef struct {
  stack_id_t      stack_id : 7;
  bool            push     : 1;
  maze_location_t value;
} queue_update_t;
#pragma pack(pop)

queue_update_t queue_updates[8];
uint8_t        queue_updates_length;

static inline maze_location_t path_peek() {
  return explorer_path_stack[explorer_path_top];
}

static inline maze_location_t path_pop() {
  maze_location_t result = path_peek();
  if (queue_updates_length < 16) {
    queue_updates[queue_updates_length++] = (queue_update_t){.stack_id = STACK_PATH, .push = false, .value = result};
  }
  explorer_path_stack[explorer_path_top--] = 0;
  return result;
}

static inline void path_push(maze_location_t loc) {
  explorer_path_stack[++explorer_path_top] = loc;
  if (queue_updates_length < 16) {
    queue_updates[queue_updates_length++] = (queue_update_t){.stack_id = STACK_PATH, .push = true, .value = loc};
  }
}

static inline maze_location_t next_peek() {
  return explorer_next_stack[explorer_next_top];
}

static inline maze_location_t next_pop() {
  maze_location_t result = next_peek();
  if (queue_updates_length < 16) {
    queue_updates[queue_updates_length++] = (queue_update_t){.stack_id = STACK_NEXT, .push = false, .value = result};
  }
  explorer_next_stack[explorer_next_top--] = 0;
  return result;
}

static inline void next_push(maze_location_t loc) {
  explorer_next_stack[++explorer_next_top] = loc;
  if (queue_updates_length < 16) {
    queue_updates[queue_updates_length++] = (queue_update_t){.stack_id = STACK_NEXT, .push = true, .value = loc};
  }
}

void          stop();
void          face(orientation_t orientation);
void          advance(maze_location_t loc, bool update_path);
orientation_t adjacent(maze_location_t a, maze_location_t b);
void          classify(maze_location_t loc);
void          update_location();

void explore() {
  // Idle the mouse and turn on the IR LEDs.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IR, .data.ir = {true}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is AXLE_OFFSET from the wall, in cell (0, 0).
  explore_orientation = NORTH;
  explore_cell_offset = AXLE_OFFSET;
  explore_stopped     = true;

  // Our path so far is just the starting square, and we want to visit the square to our north.
  explorer_path_top = 0xFF;
  explorer_next_top = 0xFF;
  path_push(maze_location(0, 0));
  next_push(maze_location(0, 1));

  // While we have squares to visit...
  while (explorer_next_top != 0xFF) {
    maze_location_t next = next_peek();
    maze_location_t curr = path_peek();

    while (maze.cells[next].visited) {
      next_pop();
      next = next_peek();
    }

    orientation_t next_orientation = adjacent(curr, next);

    // If we are adjacent to the next square...
    if (next_orientation != INVALID) {
      // Then move to it and update our map.
      next_pop();
      face(next_orientation);
      advance(next, true);
      classify(next);
    } else {
      // Otherwise, backtrack a square.
      path_pop();
      maze_location_t prev = path_peek();
      face(adjacent(curr, prev));
      advance(prev, false);
    }
  }

  // Go back to the starting cell
  while (explorer_path_top != 0) {
    maze_location_t curr = path_pop();
    maze_location_t prev = path_peek();
    face(adjacent(curr, prev));
    advance(prev, false);
  }

  // Stop in the middle of the last square.
  stop();
  face(NORTH);

  // Return to idling.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
}

// explore_report() is the report handler for the explore mode.
uint8_t explore_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_EXPLORE + 0, buffer != NULL);
  assert(ASSERT_EXPLORE + 1, len >= (sizeof(queue_update_t) * 16));

  queue_update_t *updates = (queue_update_t *)buffer;

  uint8_t report_len = queue_updates_length * sizeof(queue_update_t);
  for (int i = 0; i < queue_updates_length; i++) {
    updates[i] = queue_updates[i];
  }
  queue_updates_length = 0;

  return report_len;
}

orientation_t adjacent(maze_location_t a, maze_location_t b) {
  uint8_t ax = maze_x(a);
  uint8_t ay = maze_y(a);
  uint8_t bx = maze_x(b);
  uint8_t by = maze_y(b);

  if (ax == bx) {
    if (ay + 1 == by) {
      if (maze.cells[a].wall_north) {
        return INVALID;
      } else {
        return NORTH;
      }
    } else if (ay - 1 == by) {
      if (maze.cells[a].wall_south) {
        return INVALID;
      } else {
        return SOUTH;
      }
    } else {
      return INVALID;
    }
  } else if (ay == by) {
    if (ax + 1 == bx) {
      if (maze.cells[a].wall_east) {
        return INVALID;
      } else {
        return EAST;
      }
    } else if (ax - 1 == bx) {
      if (maze.cells[a].wall_west) {
        return INVALID;
      } else {
        return WEST;
      }
    } else {
      return INVALID;
    }
  } else {
    return INVALID;
  }
}

// face positions the mouse in the middle of the current cell, facing the given orientation.
void face(orientation_t orientation) {
  assert(ASSERT_EXPLORE + 2, orientation != INVALID);

  if (orientation == explore_orientation) {
    return;
  }

  // stop in the middle of the cell
  stop();

  uint8_t delta = (orientation + 4) - explore_orientation;
  if (delta >= 4) {
    delta -= 4;
  }

  switch (delta) {
    case 0:
      break;
    case 1:
      plan_submit_and_wait(  //
          &(plan_t){.type            = PLAN_TYPE_ROTATIONAL_MOTION,
                    .data.rotational = {
                        .d_theta = -M_PI_2,
                    }});
      break;
    case 2:
      plan_submit_and_wait(  //
          &(plan_t){.type            = PLAN_TYPE_ROTATIONAL_MOTION,
                    .data.rotational = {
                        .d_theta = M_PI,
                    }});
      break;
    case 3:
      plan_submit_and_wait(  //
          &(plan_t){.type            = PLAN_TYPE_ROTATIONAL_MOTION,
                    .data.rotational = {
                        .d_theta = M_PI_2,
                    }});
      break;
  }

  update_location();

  // assuming we were centered horizontally in the previous direction of travel
  // then we are now in the middle of the cell along the new direction of travel.
  explore_cell_offset = CELL_SIZE_2;
  explore_orientation = orientation;
}

// advance moves 1 square in the current direction, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void advance(maze_location_t loc, bool update_path) {
  update_location();

  plan_submit_and_wait(  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,
                .data.linear = {
                    .distance = CELL_SIZE - (explore_cell_offset - ENTRY_OFFSET),
                    .stop     = false,
                }});

  update_location();
  if (update_path) {
    path_push(loc);
  }
  explore_stopped = false;
}

// stop stops the mouse in the middle of the current cell.
void stop() {
  if (explore_stopped) {
    return;
  }

  update_location();

  assert(ASSERT_EXPLORE + 3, explore_cell_offset <= CELL_SIZE_2);

  // stop at the center of the cell
  plan_submit_and_wait(                                  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                .data.linear = {
                    .distance = CELL_SIZE_2 - explore_cell_offset,
                    .stop     = true  //
                }});

  update_location();
  explore_stopped = true;
}

// update_location updates the cell index and offset based on the traveled distance.
void update_location() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    explore_cell_offset += position_distance;
    position_clear();
  }

  while (explore_cell_offset > CELL_SIZE) {
    explore_cell_offset -= CELL_SIZE;
  }
}

void queue(maze_location_t loc) {
  if (!maze.cells[loc].visited) {
    next_push(loc);
  }
}

// classify updates the state of a cell.
// assumption: we are at ENTRY_OFFSET into the cell.
void classify(maze_location_t loc) {
  assert(ASSERT_EXPLORE + 4, explore_orientation != INVALID);

  bool wall_forward, wall_left, wall_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_forward = wall_forward_present;
    wall_left    = wall_left_present;
    wall_right   = wall_right_present;
  }

  // Classify the square based on sensor readings.
  cell_t cell  = {0};
  cell.visited = true;
  switch (explore_orientation) {
    case NORTH:
      if (wall_right) {
        cell.wall_east = wall_right;
      } else {
        queue(loc + maze_location(1, 0));
      }
      if (wall_left) {
        cell.wall_west = wall_left;
      } else {
        queue(loc - maze_location(1, 0));
      }
      if (wall_forward) {
        cell.wall_north = wall_forward;
      } else {
        queue(loc + maze_location(0, 1));
      }
      break;
    case EAST:
      if (wall_right) {
        cell.wall_south = wall_right;
      } else {
        queue(loc - maze_location(0, 1));
      }
      if (wall_left) {
        cell.wall_north = wall_left;
      } else {
        queue(loc + maze_location(0, 1));
      }
      if (wall_forward) {
        cell.wall_east = wall_forward;
      } else {
        queue(loc + maze_location(1, 0));
      }
      break;
    case SOUTH:
      if (wall_right) {
        cell.wall_west = wall_right;
      } else {
        queue(loc - maze_location(1, 0));
      }
      if (wall_left) {
        cell.wall_east = wall_left;
      } else {
        queue(loc + maze_location(1, 0));
      }
      if (wall_forward) {
        cell.wall_south = wall_forward;
      } else {
        queue(loc - maze_location(0, 1));
      }
      break;
    case WEST:
      if (wall_right) {
        cell.wall_north = wall_right;
      } else {
        queue(loc + maze_location(0, 1));
      }
      if (wall_left) {
        cell.wall_south = wall_left;
      } else {
        queue(loc - maze_location(0, 1));
      }
      if (wall_forward) {
        cell.wall_west = wall_forward;
      } else {
        queue(loc - maze_location(1, 0));
      }
      break;
    case INVALID:
      break;
  }

  maze_update(loc, cell);
}
