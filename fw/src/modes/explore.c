#include <avr/interrupt.h>
#include <avr/sleep.h>
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
static uint8_t       explore_x, explore_y;

void explore() {
  plan_submit_and_wait(                       //
      &(plan_t){.type      = PLAN_TYPE_LEDS,  //
                .data.leds = {false, false, false}});
  plan_submit_and_wait(                   //
      &(plan_t){.type    = PLAN_TYPE_IR,  //
                .data.ir = {true}});
  plan_submit_and_wait(                               //
      &(plan_t){.type       = PLAN_TYPE_FIXED_POWER,  //
                .data.power = {0, 0}});

  // Assumption:
  // We start centered along the back wall of the starting square, with our back touching the wall.
  // Therefore our "position", measured by the center of the axle is AXLE_OFFSET from the wall.
  explore_orientation = NORTH;
  explore_cell_offset = AXLE_OFFSET;

  // Start by advancing to where our sensors are pointed at the walls of the next square.
  float temp;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    temp = position_distance;
  }

  plan_submit_and_wait(                                  //
      &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,  //
                .data.linear = {
                    .distance = 196.0 - explore_cell_offset,
                    .stop     = true  //
                }});

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    explore_cell_offset += position_distance - temp;
    while (explore_cell_offset > 180.0) {
      explore_cell_offset -= 180.0;
      explore_y++;
    }

    // Now we can classify the square based on sensor readings.
    cell_t cell;
    cell.wall_north = wall_forward_present;
    cell.wall_east  = wall_right_present;
    cell.wall_south = false;
    cell.wall_west  = wall_left_present;
    maze_update(explore_x, explore_y, cell);
  }

  // And that's all we got for now.
  plan_submit_and_wait(                   //
      &(plan_t){.type    = PLAN_TYPE_IR,  //
                .data.ir = {false}});
}
