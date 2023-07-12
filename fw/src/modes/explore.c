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

// update_location updates the cell index and offset based on the traveled distance.
void update_location() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    explore_cell_offset += position_distance;
    position_clear();
  }

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

  assert(ASSERT_EXPLORE + 0, explore_cell_offset <= 80.0);

  update_location();

  // stop at the center of the cell
  plan_submit_and_wait(                                  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                .data.linear = {
                    .distance = 80.0 - explore_cell_offset,
                    .stop     = true  //
                }});

  update_location();
  explore_stopped = true;
}

// turn_to positions the mouse in the middle of the current cell, facing the given orientation.
void turn_to(orientation_t orientation) {
  assert(ASSERT_EXPLORE + 1, orientation < 4);

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
  explore_cell_offset = 80.0;
  explore_orientation = orientation;
}

// drive_straight_to drives straight to the given cell, leaving the mouse
// in a position where the sensors are pointed at the centers of the side walls.
void drive_straight_to(uint8_t x, uint8_t y) {
  assert(ASSERT_EXPLORE + 2, x < MAZE_WIDTH);
  assert(ASSERT_EXPLORE + 3, y < MAZE_HEIGHT);
  assert(ASSERT_EXPLORE + 4, explore_cell_x == x || explore_cell_y == y);

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

  update_location();

  plan_submit_and_wait(  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,
                .data.linear = {
                    .distance = (cells * 180.0) - (explore_cell_offset - ENTRY_OFFSET),
                    .stop     = false,
                }});

  update_location();
  explore_stopped = false;
}

// update_cell updates the state of a cell.
// assumption: we are at ENTRY_OFFSET into the cell.
void update_cell() {
  bool wall_forward, wall_left, wall_right;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_forward = wall_forward_present;
    wall_left    = wall_left_present;
    wall_right   = wall_right_present;
  }

  // Classify the square based on sensor readings.
  cell_t cell;
  switch (explore_orientation) {
    case NORTH:
      cell.wall_north = wall_forward;
      cell.wall_east  = wall_right;
      cell.wall_south = false;
      cell.wall_west  = wall_left;
      break;
    case EAST:
      cell.wall_north = wall_left;
      cell.wall_east  = wall_forward;
      cell.wall_south = wall_right;
      cell.wall_west  = false;
      break;
    case SOUTH:
      cell.wall_north = false;
      cell.wall_east  = wall_left;
      cell.wall_south = wall_forward;
      cell.wall_west  = wall_right;
      break;
    case WEST:
      cell.wall_north = wall_right;
      cell.wall_east  = false;
      cell.wall_south = wall_left;
      cell.wall_west  = wall_forward;
      break;
  }

  maze_update(explore_cell_x, explore_cell_y, cell);
}

void drive_straight1() {
  switch (explore_orientation) {
    case NORTH:
      drive_straight_to(explore_cell_x, explore_cell_y + 1);
      break;
    case EAST:
      drive_straight_to(explore_cell_x + 1, explore_cell_y);
      break;
    case SOUTH:
      drive_straight_to(explore_cell_x, explore_cell_y - 1);
      break;
    case WEST:
      drive_straight_to(explore_cell_x - 1, explore_cell_y);
      break;
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

  while (true) {
    // Until we hit the end of the corridor...
    while (!wall_forward_present) {
      drive_straight1();
      update_cell();
    }

    if (!wall_left_present) {
      turn_to((explore_orientation - 1) & 0x03);
    } else if (!wall_right_present) {
      turn_to((explore_orientation + 1) & 0x03);
    } else {
      break;
    }
  }

  stop();

  // Return to idling.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
}
