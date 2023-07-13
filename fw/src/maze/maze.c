#include "maze.h"

#include <string.h>

#include "utils/assert.h"

maze_t  maze;
uint8_t maze_report_row;

static maze_location_t maze_update_queue[16];
static uint8_t         maze_update_queue_length;

void maze_init() {
  maze_report_row = MAZE_HEIGHT;

  // Distances default to 0xFF, which is the maximum possible distance.
  for (uint16_t xy = 0; xy < (MAZE_WIDTH * MAZE_HEIGHT); xy++) {
    maze.cells[xy].distance = 0xFF;
  }
  // Bottom and top rows always have outer walls.
  for (uint8_t x = 0; x < MAZE_WIDTH; x++) {
    maze.cells[maze_location(x, 0)].wall_south               = true;
    maze.cells[maze_location(x, MAZE_HEIGHT - 1)].wall_north = true;
  }
  // Left and right columns always have outer walls.
  for (int y = 0; y < MAZE_HEIGHT; y++) {
    maze.cells[maze_location(0, y)].wall_west              = true;
    maze.cells[maze_location(MAZE_WIDTH - 1, y)].wall_east = true;
  }
  // The starting cell always has a wall to the east.
  maze.cells[maze_location(0, 0)].wall_east = true;
  // The starting cell has a distance of 0.
  maze.cells[maze_location(0, 0)].distance = 0;
  // The starting cell has been visited (we are there now).
  maze.cells[maze_location(0, 0)].visited = true;
}

void maze_send() {
  maze_update_queue_length = 0;
  maze_report_row          = 0;
}

uint8_t maze_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_MAZE + 0, buffer != NULL);
  assert(ASSERT_MAZE + 0, len >= (sizeof(maze_update_t) * MAZE_WIDTH));

  maze_update_t *updates = (maze_update_t *)buffer;

  if (maze_report_row < MAZE_HEIGHT) {
    for (int i = 0; i < MAZE_WIDTH; i++) {
      updates[i] = (maze_update_t){
          .location = maze_location(i, maze_report_row),
          .cell     = maze.cells[maze_location(i, maze_report_row)],
      };
    }
    maze_report_row++;
    return MAZE_WIDTH * sizeof(maze_update_t);
  }

  uint8_t report_len = maze_update_queue_length * sizeof(maze_update_t);
  for (int i = 0; i < maze_update_queue_length; i++) {
    maze_location_t loc = maze_update_queue[i];
    updates[i]          = (maze_update_t){.location = loc, .cell = maze.cells[loc]};
  }
  maze_update_queue_length = 0;

  return report_len;
}

void maze_update(maze_location_t loc, cell_t cell) {
  assert(ASSERT_MAZE + 0, maze_x(loc) < MAZE_WIDTH);
  assert(ASSERT_MAZE + 1, maze_y(loc) < MAZE_HEIGHT);

  maze.cells[loc] = cell;

  if (maze_update_queue_length < 16) {
    maze_update_queue[maze_update_queue_length++] = loc;
  } else {
    maze_send();
  }
}
