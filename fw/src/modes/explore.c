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
#include "modes/explore_int.h"
#include "modes/remote.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"
#include "utils/assert.h"
#include "utils/dequeue.h"

static orientation_t explore_orientation;  // The current orientation of the mouse.
static float         explore_cell_offset;  // The offset into the current cell.
static bool          explore_stopped;      // Whether or not the mouse has stopped.

DEFINE_DEQUEUE(maze_location_t, path, 256);     // The breadcrumb trail.
DEFINE_DEQUEUE(maze_location_t, next, 256);     // The stack of unvisited cells.
DEFINE_DEQUEUE(dequeue_update_t, updates, 16);  // The queue of updates to send to the host.

// path_queue_callback is the callback for changes the path queue.
void path_queue_callback(dequeue_event_type_t event, maze_location_t value) {
  if (!updates_full()) {
    updates_push_back((dequeue_update_t){.dequeue_id = DEQUEUE_PATH, .event = event, .value = value});
  }
}

// next_queue_callback is the callback for changes the next queue.
void next_queue_callback(dequeue_event_type_t event, maze_location_t value) {
  if (!updates_full()) {
    updates_push_back((dequeue_update_t){.dequeue_id = DEQUEUE_NEXT, .event = event, .value = value});
  }
}

void explore_init() {
  path_init();
  next_init();
  updates_init();
  path_register_callback(path_queue_callback);
  next_register_callback(next_queue_callback);
}

void explore() {
  // Idle the mouse and turn on the IR LEDs.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IR, .data.ir = {true}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is AXLE_OFFSET from the wall.
  explore_orientation = NORTH;
  explore_cell_offset = AXLE_OFFSET;
  explore_stopped     = true;

  // Our path so far is just the starting square, and we want to visit the square to our north.
  path_push_back(maze_location(0, 0));
  next_push_back(maze_location(0, 1));

  // While we have squares to visit...
  while (!next_empty()) {
    // Skips cells we already visited since they were added to the stack.
    while (!next_empty() && maze_read(next_peek_back()).visited) {
      next_pop_back();
    }

    // If we have no more cells to visit, then we are done.
    if (next_empty()) {
      break;
    }

    maze_location_t curr = path_peek_back();
    maze_location_t next = next_peek_back();
    maze_location_t prev;

    // Determine which direction to drive to reach the cell (if it is adjancent).
    orientation_t next_orientation = adjacent(curr, next);

    // If we are adjacent to the next cell
    if (next_orientation != INVALID) {
      next_pop_back();         // Remove the cell from the "next" stack.
      face(next_orientation);  // Turn to face the cell.
      advance(next, true);     // Advance into it, updating the breadcrumb trail.
      classify(next);          // Update our maze representation, and note any new cells to visit.
    } else {
      // Otherwise, backtrack a square.
      path_pop_back();             // Remove the current cell from the breadcrumb trail.
      prev = path_peek_back();     // Check which cell we came from.
      face(adjacent(curr, prev));  // Turn to face it.
      advance(prev, false);        // Advance into it, but do NOT update the breadcrumb trail.
    }
  }

  // Now that exploration is complete, we return to the starting cell.
  if (!path_empty()) {
    maze_location_t curr = path_pop_back();
    while (!path_empty()) {
      maze_location_t prev = path_pop_back();
      face(adjacent(curr, prev));
      advance(prev, false);
      curr = prev;
    }
  }

  // Stop in the middle of the last square, facing north.
  stop();
  face(NORTH);

  // Ensure the control system is idling (no motor activity).
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});

  // Deregister dequeue callbacks.
  path_register_callback(NULL);
  next_register_callback(NULL);

  // Solve the maze, and tramit the new maze data to the host.
  floodfill();
  maze_send();
}

// explore_report() is the report handler for the explore mode.
uint8_t explore_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_EXPLORE + 0, buffer != NULL);
  assert(ASSERT_EXPLORE + 1, len >= (sizeof(dequeue_update_t) * 16));

  uint8_t i          = 0;
  uint8_t report_len = 0;

  dequeue_update_t *updates = (dequeue_update_t *)buffer;
  while (!updates_empty()) {
    updates[i++]  = updates_pop_front();
    report_len   += sizeof(dequeue_update_t);
  }

  return report_len;
}

// adjacent determines the orientation needed to drive between two adjacent cells.
orientation_t adjacent(maze_location_t a, maze_location_t b) {
  uint8_t ax = maze_x(a);
  uint8_t ay = maze_y(a);
  uint8_t bx = maze_x(b);
  uint8_t by = maze_y(b);

  if (ax == bx) {
    if (ay + 1 == by) {
      if (maze_read(a).wall_north) {
        return INVALID;
      } else {
        return NORTH;
      }
    } else if (ay - 1 == by) {
      if (maze_read(a).wall_south) {
        return INVALID;
      } else {
        return SOUTH;
      }
    } else {
      return INVALID;
    }
  } else if (ay == by) {
    if (ax + 1 == bx) {
      if (maze_read(a).wall_east) {
        return INVALID;
      } else {
        return EAST;
      }
    } else if (ax - 1 == bx) {
      if (maze_read(a).wall_west) {
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

  // determine the delta between the current orientation and the desired orientation
  // Note: The (+4) and (&3) is to ensure the result is in [0,3] without using an expensive % operator.
  uint8_t delta  = (orientation + 4) - explore_orientation;
  delta         &= 3;

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

  float position, theta;
  position_tare(&position, &theta);

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
                    .position = CELL_SIZE - (explore_cell_offset - ENTRY_OFFSET),
                    .stop     = false,
                }});

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

  // we should never decide to stop once we have already passed the center of a cell.
  assert(ASSERT_EXPLORE + 3, explore_cell_offset <= CELL_SIZE_2);

  // stop at the center of the cell
  plan_submit_and_wait(                                  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                .data.linear = {
                    .position = CELL_SIZE_2 - explore_cell_offset,
                    .stop     = true  //
                }});

  explore_stopped = true;
}

// update_location updates the cell index and offset based on the traveled distance.
void update_location() {
  float position_distance, position_theta;
  position_tare(&position_distance, &position_theta);
  explore_cell_offset += position_distance;

  // this is a (% CELL_SIZE) without using the % operator because it is expensive.
  while (explore_cell_offset > CELL_SIZE) {
    explore_cell_offset -= CELL_SIZE;
  }
}

// queue_unvisited adds a cell to the "next" stack if it has not already been visited.
void queue_unvisited(maze_location_t loc) {
  if (!maze_read(loc).visited) {
    next_push_back(loc);
  }
}

// classify updates the state of a cell, and adds unvisited neighbors to the "next" stack.
void classify(maze_location_t loc) {
  assert(ASSERT_EXPLORE + 4, explore_orientation != INVALID);

  bool wall_forward, wall_left, wall_right;
  walls_present(&wall_left, &wall_right, &wall_forward);

  // Classify the square based on sensor readings.
  maze_cell_t cell = {
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
        queue_unvisited(loc + maze_location(1, 0));
      }
      if (wall_left) {
        cell.wall_west = wall_left;
      } else {
        queue_unvisited(loc - maze_location(1, 0));
      }
      if (wall_forward) {
        cell.wall_north = wall_forward;
      } else {
        queue_unvisited(loc + maze_location(0, 1));
      }
      break;
    case EAST:
      if (wall_right) {
        cell.wall_south = wall_right;
      } else {
        queue_unvisited(loc - maze_location(0, 1));
      }
      if (wall_left) {
        cell.wall_north = wall_left;
      } else {
        queue_unvisited(loc + maze_location(0, 1));
      }
      if (wall_forward) {
        cell.wall_east = wall_forward;
      } else {
        queue_unvisited(loc + maze_location(1, 0));
      }
      break;
    case SOUTH:
      if (wall_right) {
        cell.wall_west = wall_right;
      } else {
        queue_unvisited(loc - maze_location(1, 0));
      }
      if (wall_left) {
        cell.wall_east = wall_left;
      } else {
        queue_unvisited(loc + maze_location(1, 0));
      }
      if (wall_forward) {
        cell.wall_south = wall_forward;
      } else {
        queue_unvisited(loc - maze_location(0, 1));
      }
      break;
    case WEST:
      if (wall_right) {
        cell.wall_north = wall_right;
      } else {
        queue_unvisited(loc + maze_location(0, 1));
      }
      if (wall_left) {
        cell.wall_south = wall_left;
      } else {
        queue_unvisited(loc - maze_location(0, 1));
      }
      if (wall_forward) {
        cell.wall_west = wall_forward;
      } else {
        queue_unvisited(loc - maze_location(1, 0));
      }
      break;
    case INVALID:
      break;
  }

  maze_update(loc, cell);
}

// floodfill calculates the shortest path to the goal.
void floodfill() {
  // Step 1. Find the 2x2 square of cells with no internal walls that is the goal.
  maze_location_t goal = maze_location(15, 15);
  for (uint8_t x = 0; x < MAZE_WIDTH - 1; x++) {
    for (uint8_t y = 0; y < MAZE_HEIGHT - 1; y++) {
      maze_cell_t a = maze_read(maze_location(x, y));
      maze_cell_t b = maze_read(maze_location(x + 1, y + 1));
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

  // Step 2. Find the cell in the goal square with the gateway, as that's where we want to get.
  if (!maze_read(goal + maze_location(0, 1)).wall_north || !maze_read(goal + maze_location(0, 1)).wall_west) {
    goal += maze_location(0, 1);
  } else if (!maze_read(goal + maze_location(1, 0)).wall_south || !maze_read(goal + maze_location(1, 0)).wall_east) {
    goal += maze_location(1, 0);
  } else if (!maze_read(goal + maze_location(1, 1)).wall_north || !maze_read(goal + maze_location(1, 1)).wall_east) {
    goal += maze_location(1, 1);
  }

  // Step 3. Floodfill outwards from the goal cell.
  path_clear();
  maze_cell_t goal_cell = maze_read(goal);
  goal_cell.distance    = 0;
  maze_update(goal, goal_cell);
  path_push_back(goal);
  while (!path_empty()) {
    maze_location_t loc  = path_pop_front();
    maze_cell_t     cell = maze_read(loc);
    if (!cell.wall_north) {
      maze_location_t next      = loc + maze_location(0, 1);
      maze_cell_t     next_cell = maze_read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze_update(next, next_cell);
        path_push_back(next);
      }
    }
    if (!cell.wall_east) {
      maze_location_t next      = loc + maze_location(1, 0);
      maze_cell_t     next_cell = maze_read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze_update(next, next_cell);
        path_push_back(next);
      }
    }
    if (!cell.wall_south) {
      maze_location_t next      = loc - maze_location(0, 1);
      maze_cell_t     next_cell = maze_read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze_update(next, next_cell);
        path_push_back(next);
      }
    }
    if (!cell.wall_west) {
      maze_location_t next      = loc - maze_location(1, 0);
      maze_cell_t     next_cell = maze_read(next);
      if (next_cell.distance == 0xFF) {
        next_cell.distance = cell.distance + 1;
        maze_update(next, next_cell);
        path_push_back(next);
      }
    }
  }
}

// solve follows the shortest path to the goal.
void solve() {
  maze_location_t curr     = maze_location(0, 0);
  uint8_t         distance = maze_read(curr).distance;

  while (distance != 0) {
    maze_location_t north = curr + maze_location(0, 1);
    maze_location_t east  = curr + maze_location(1, 0);
    maze_location_t south = curr - maze_location(0, 1);
    maze_location_t west  = curr - maze_location(1, 0);

    maze_location_t next = curr;

    if (maze_x(curr) < (MAZE_HEIGHT - 1) && !maze_read(curr).wall_north && maze_read(north).distance < distance) {
      next     = north;
      distance = maze_read(north).distance;
    }

    if (maze_y(curr) < (MAZE_WIDTH - 1) && !maze_read(curr).wall_east && maze_read(east).distance < distance) {
      next     = east;
      distance = maze_read(east).distance;
    }

    if (maze_x(curr) > 0 && !maze_read(curr).wall_south && maze_read(south).distance < distance) {
      next     = south;
      distance = maze_read(south).distance;
    }

    if (maze_y(curr) > 0 && !maze_read(curr).wall_west && maze_read(west).distance < distance) {
      next     = west;
      distance = maze_read(west).distance;
    }

    if (next == curr) {
      // No path to the goal.
      return;
    }

    face(adjacent(curr, next));
    advance(next, false);
    curr = next;
  }

  // Stop in the middle of the goal cell.
  stop();

  // Ensure the control system is idling (no motor activity).
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
}
