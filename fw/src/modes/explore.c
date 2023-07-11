#include <avr/interrupt.h>
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

typedef enum : uint8_t { NORTH, EAST, SOUTH, WEST } orientation_t;

const float ENTRY_OFFSET = 16.0;

static float         explore_cell_offset;
static orientation_t explore_orientation;
static uint8_t       explore_cell_x, explore_cell_y;
static bool          explore_stopped;

// update_location updates the cell index and offset based on the given distance.
void update_location(float distance) {
  explore_cell_offset += distance;
  while (explore_cell_offset > 180.0) {
    explore_cell_offset -= 180.0;
    switch (explore_orientation) {
      case NORTH:
        explore_cell_y++;
        break;
      case EAST:
        explore_cell_x++;
        break;
      case SOUTH:
        explore_cell_y--;
        break;
      case WEST:
        explore_cell_x--;
        break;
    }
  }
}

// stop stops the mouse in the middle of the current cell.
void stop() {
  if (explore_stopped) {
    return;
  }

  float start, end;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    start = position_distance;
  }

  if (explore_cell_offset >= 80.0) {
    // if we are already past the middle of the cell then just stop here.
    plan_submit_and_wait(                               //
        &(plan_t){.type       = PLAN_TYPE_FIXED_POWER,  //
                  .data.power = {0, 0}});
  } else {
    // otherwise drive there and stop.
    plan_submit_and_wait(                                  //
        &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                  .data.linear = {
                      .distance = 80.0 - explore_cell_offset,
                      .stop     = true  //
                  }});
  }

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    end = position_distance;
  }

  update_location(end - start);
  explore_stopped = true;
}

// turn_to positions the mouse in the middle of the current cell, facing the given orientation.
void turn_to(orientation_t orientation) {
  assert(ASSERT_EXPLORE + 0, orientation < 4);

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

  explore_orientation = orientation;
}

// drive_straight_to drives straight to the given cell, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void drive_straight_to(uint8_t x, uint8_t y) {
  assert(ASSERT_EXPLORE + 0, x < MAZE_WIDTH);
  assert(ASSERT_EXPLORE + 1, y < MAZE_HEIGHT);
  assert(ASSERT_EXPLORE + 2, explore_cell_x == x || explore_cell_y == y);

  uint8_t cells = 0;
  if (x > explore_cell_x) {
    turn_to(EAST);
    cells = x - explore_cell_x;
  } else if (x < explore_cell_x) {
    turn_to(WEST);
    cells = explore_cell_x - x;
  } else if (y > explore_cell_y) {
    turn_to(NORTH);
    cells = y - explore_cell_y;
  } else if (y < explore_cell_y) {
    turn_to(SOUTH);
    cells = explore_cell_y - y;
  }

  float start, end;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    start = position_distance;
  }

  plan_submit_and_wait(  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,
                .data.linear = {
                    .distance = (cells * 180.0) - (explore_cell_offset - ENTRY_OFFSET),
                    .stop     = false,
                }});

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    end = position_distance;
  }

  update_location(end - start);
  explore_stopped = false;
}

// update_cell updates the state of a cell.
// assumption: we are at ENTRY_OFFSET into the cell.
void update_cell() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Classify the square based on sensor readings.
    cell_t cell;
    switch (explore_orientation) {
      case NORTH:
        cell.wall_north = wall_forward_present;
        cell.wall_east  = wall_right_present;
        cell.wall_south = false;
        cell.wall_west  = wall_left_present;
        break;
      case EAST:
        cell.wall_north = wall_left_present;
        cell.wall_east  = wall_forward_present;
        cell.wall_south = wall_right_present;
        cell.wall_west  = false;
        break;
      case SOUTH:
        cell.wall_north = false;
        cell.wall_east  = wall_left_present;
        cell.wall_south = wall_forward_present;
        cell.wall_west  = wall_right_present;
        break;
      case WEST:
        cell.wall_north = wall_right_present;
        cell.wall_east  = false;
        cell.wall_south = wall_left_present;
        cell.wall_west  = wall_forward_present;
        break;
    }
    cell.distance = 0xFE;
    if (!cell.wall_north) {
      cell.distance = min8(cell.distance, maze.cells[explore_cell_x][explore_cell_y + 1].distance + 1);
    }
    if (!cell.wall_east) {
      cell.distance = min8(cell.distance, maze.cells[explore_cell_x + 1][explore_cell_y].distance + 1);
    }
    if (!cell.wall_south) {
      cell.distance = min8(cell.distance, maze.cells[explore_cell_x][explore_cell_y - 1].distance + 1);
    }
    if (!cell.wall_east) {
      cell.distance = min8(cell.distance, maze.cells[explore_cell_x - 1][explore_cell_y].distance + 1);
    }
    maze_update(explore_cell_x, explore_cell_y, cell);
  }
}

void explore() {
  // Idle the mouse and turn on the IR LEDs.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IR, .data.ir = {true}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is AXLE_OFFSET from the wall, in cell (0, 0).
  explore_orientation = NORTH;
  explore_cell_offset = AXLE_OFFSET;
  explore_cell_x      = 0;
  explore_cell_y      = 0;
  explore_stopped     = true;

  // Until we hit the end of the corridor...
  while (!wall_forward_present) {
    drive_straight_to(explore_cell_x, explore_cell_y + 1);
    update_cell();
  }

  stop();

  // Return to idling.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
}
