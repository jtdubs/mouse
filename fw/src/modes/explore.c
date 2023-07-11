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

typedef enum : uint8_t { NORTH, EAST, SOUTH, WEST } orientation_t;

static float         explore_cell_offset;
static orientation_t explore_orientation;
static uint8_t       explore_cell_x, explore_cell_y;

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

  // Until we hit the end of the corridor...
  while (!wall_forward_present) {
    // Make note of our current position.
    float temp;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      temp = position_distance;
    }

    // Advance to where our sensors are pointed at the walls of the next square.
    plan_submit_and_wait(                                  //
        &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                  .data.linear = {
                      .distance = 196.0 - explore_cell_offset,
                      .stop     = false  //
                  }});

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // Update our cell index and offset.
      // TODO: switch(explore_orientation)
      explore_cell_offset += position_distance - temp;
      while (explore_cell_offset > 180.0) {
        explore_cell_offset -= 180.0;
        explore_cell_y++;
      }

      // Classify the square based on sensor readings.
      // TODO: switch(explore_orientation)
      cell_t cell;
      cell.wall_north = wall_forward_present;
      cell.wall_east  = wall_right_present;
      cell.wall_south = false;
      cell.wall_west  = wall_left_present;
      cell.distance   = 0;
      maze_update(explore_cell_x, explore_cell_y, cell);
    }
  }

  // Stop in the middle of the last cell of the corridor.
  plan_submit_and_wait(                                  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                .data.linear = {
                    .distance = 80.0 - explore_cell_offset,
                    .stop     = true  //
                }});

  // Return to idling.
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});
}
