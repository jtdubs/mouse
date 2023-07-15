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
#include "utils/dequeue.h"

typedef enum : uint8_t { NORTH, EAST, SOUTH, WEST, INVALID } orientation_t;

constexpr float ENTRY_OFFSET = 16.0;               // mm
constexpr float CELL_SIZE    = 180.0;              // mm
constexpr float CELL_SIZE_2  = (CELL_SIZE / 2.0);  // mm

static orientation_t explore_orientation;
static float         explore_cell_offset;
static bool          explore_stopped;

typedef enum : uint8_t {
  STACK_PATH,
  STACK_NEXT,
} stack_id_t;

#pragma pack(push, 1)
typedef struct {
  stack_id_t           stack_id : 6;
  dequeue_event_type_t event    : 2;
  maze_location_t      value;
} queue_update_t;
#pragma pack(pop)

DEFINE_DEQUEUE(maze_location_t, path, 256);
DEFINE_DEQUEUE(maze_location_t, next, 256);
DEFINE_DEQUEUE(queue_update_t, updates, 16);

void          stop();
void          face(orientation_t orientation);
void          advance(maze_location_t loc, bool update_path);
orientation_t adjacent(maze_location_t a, maze_location_t b);
void          classify(maze_location_t loc);
void          update_location();
void          solve();

void path_queue_callback(dequeue_event_type_t event, maze_location_t value) {
  if (!updates_full()) {
    updates_push_back((queue_update_t){.stack_id = STACK_PATH, .event = event, .value = value});
  }
}

void next_queue_callback(dequeue_event_type_t event, maze_location_t value) {
  if (!updates_full()) {
    updates_push_back((queue_update_t){.stack_id = STACK_NEXT, .event = event, .value = value});
  }
}

void explore() {
  // Register dequeue callbacks.
  path_register_callback(path_queue_callback);
  next_register_callback(next_queue_callback);

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
  path_push_back(maze_location(0, 0));
  next_push_back(maze_location(0, 1));

  // While we have squares to visit...
  while (!next_empty()) {
    maze_location_t next = next_peek_back();
    maze_location_t curr = path_peek_back();

    while (!next_empty() && maze.cells[next].visited) {
      next_pop_back();
      next = next_peek_back();
    }

    if (next_empty()) {
      break;
    }

    orientation_t next_orientation = adjacent(curr, next);

    // If we are adjacent to the next square...
    if (next_orientation != INVALID) {
      // Then move to it and update our map.
      next_pop_back();
      face(next_orientation);
      advance(next, true);
      classify(next);
    } else {
      // Otherwise, backtrack a square.
      path_pop_back();
      maze_location_t prev = path_peek_back();
      face(adjacent(curr, prev));
      advance(prev, false);
    }
  }

  // Go back to the starting cell
  while (!path_empty()) {
    maze_location_t curr = path_pop_back();
    maze_location_t prev = path_peek_back();
    face(adjacent(curr, prev));
    advance(prev, false);
  }

  // Stop in the middle of the last square.
  stop();
  face(NORTH);

  // Return to idling.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});

  // Deregister dequeue callbacks.
  path_register_callback(NULL);
  next_register_callback(NULL);

  // Solve the maze.
  solve();
}

// explore_report() is the report handler for the explore mode.
uint8_t explore_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_EXPLORE + 0, buffer != NULL);
  assert(ASSERT_EXPLORE + 1, len >= (sizeof(queue_update_t) * 16));

  uint8_t i          = 0;
  uint8_t report_len = 0;

  queue_update_t *updates = (queue_update_t *)buffer;
  while (!updates_empty()) {
    updates[i++]  = updates_pop_front();
    report_len   += sizeof(queue_update_t);
  }

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
    path_push_back(loc);
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
  float position_distance, position_theta;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    position_read(&position_distance, &position_theta);
    explore_cell_offset += position_distance;
    position_clear();
  }

  while (explore_cell_offset > CELL_SIZE) {
    explore_cell_offset -= CELL_SIZE;
  }
}

void queue(maze_location_t loc) {
  if (!maze.cells[loc].visited) {
    next_push_back(loc);
  }
}

// classify updates the state of a cell.
// assumption: we are at ENTRY_OFFSET into the cell.
void classify(maze_location_t loc) {
  assert(ASSERT_EXPLORE + 4, explore_orientation != INVALID);

  bool wall_forward, wall_left, wall_right;
  walls_present(&wall_left, &wall_right, &wall_forward);

  // Classify the square based on sensor readings.
  cell_t cell = {
      .visited    = true,
      .distance   = 0xFF,
      .wall_north = false,
      .wall_east  = false,
      .wall_south = false,
      .wall_west  = false,
  };
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

void solve() {
  // Step 1. Find the 2x2 square of cells with no internal walls that is the goal.
  maze_location_t goal = maze_location(15, 15);
  for (uint8_t x = 0; x < MAZE_WIDTH - 1; x++) {
    for (uint8_t y = 0; y < MAZE_HEIGHT - 1; y++) {
      cell_t a = maze.cells[maze_location(x, y)];
      cell_t b = maze.cells[maze_location(x + 1, y + 1)];
      if (a.visited && b.visited && !a.wall_east && !a.wall_north && !b.wall_west && !b.wall_south) {
        goal = maze_location(x, y);
        break;
      }
    }
  }
  if (goal == 0xFF) {
    // No goal found.
    return;
  }

  // Step 2. Find the cell in the goal square with the gateway.
  if (!maze.cells[goal + maze_location(0, 1)].wall_north || !maze.cells[goal + maze_location(0, 1)].wall_west) {
    goal += maze_location(0, 1);
  } else if (!maze.cells[goal + maze_location(1, 0)].wall_south || !maze.cells[goal + maze_location(1, 0)].wall_east) {
    goal += maze_location(1, 0);
  } else if (!maze.cells[goal + maze_location(1, 1)].wall_north || !maze.cells[goal + maze_location(1, 1)].wall_east) {
    goal += maze_location(1, 1);
  }

  // Step 3. Floodfill outwards from the goal cell.
  path_clear();
  maze.cells[goal].distance = 0;
  path_push_back(goal);
  while (!path_empty()) {
    maze_location_t loc  = path_pop_front();
    cell_t          cell = maze.cells[loc];
    if (!cell.wall_north) {
      maze_location_t next = loc + maze_location(0, 1);
      if (maze.cells[next].distance == 0xFF) {
        maze.cells[next].distance = cell.distance + 1;
        path_push_back(next);
      }
    }
    if (!cell.wall_east) {
      maze_location_t next = loc + maze_location(1, 0);
      if (maze.cells[next].distance == 0xFF) {
        maze.cells[next].distance = cell.distance + 1;
        path_push_back(next);
      }
    }
    if (!cell.wall_south) {
      maze_location_t next = loc - maze_location(0, 1);
      if (maze.cells[next].distance == 0xFF) {
        maze.cells[next].distance = cell.distance + 1;
        path_push_back(next);
      }
    }
    if (!cell.wall_west) {
      maze_location_t next = loc - maze_location(1, 0);
      if (maze.cells[next].distance == 0xFF) {
        maze.cells[next].distance = cell.distance + 1;
        path_push_back(next);
      }
    }
  }

  // Step 4. Trigger retransmission of the maze state to the remote.
  maze_send();
}
