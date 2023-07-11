#include "maze.h"

#include <string.h>

#include "utils/assert.h"

maze_t  maze;
uint8_t report_row;

static maze_update_t maze_update_buffer[8];
static uint8_t       maze_update_buffer_length;

void maze_init() {
  report_row = MAZE_HEIGHT;

  // Distances default to 0xFF, which is the maximum possible distance.
  for (int i = 0; i < MAZE_WIDTH; i++) {
    for (int j = 0; j < MAZE_HEIGHT; j++) {
      maze.cells[i][j].distance = 0xFF;
    }
  }
  // Bottom and top rows always have outer walls.
  for (int i = 0; i < MAZE_WIDTH; i++) {
    maze.cells[i][0].wall_south               = true;
    maze.cells[i][MAZE_HEIGHT - 1].wall_north = true;
  }
  // Left and right columns always have outer walls.
  for (int i = 0; i < MAZE_HEIGHT; i++) {
    maze.cells[0][i].wall_west              = true;
    maze.cells[MAZE_WIDTH - 1][i].wall_east = true;
  }
  // The starting cell always has a wall to the east.
  maze.cells[0][0].wall_east = true;
  // The starting cell has a distance of 0.
  maze.cells[0][0].distance = 0;
}

void maze_send() {
  report_row = 0;
}

uint8_t maze_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_MAZE + 0, buffer != NULL);
  assert(ASSERT_MAZE + 0, len >= (sizeof(maze_update_t) * MAZE_WIDTH));

  if (report_row < MAZE_HEIGHT) {
    maze_update_t *updates = (maze_update_t *)buffer;

    for (int i = 0; i < MAZE_WIDTH; i++) {
      updates[i] = (maze_update_t){
          .x    = i,
          .y    = report_row,
          .cell = maze.cells[i][report_row],
      };
    }
    report_row++;
    return MAZE_WIDTH * sizeof(maze_update_t);
  }

  // TODO: only send updates that fit in the buffer, then either copy
  // remaining updates to the front, or use a ring buffer instead.
  if (maze_update_buffer_length > 0) {
    size_t len = maze_update_buffer_length * sizeof(maze_update_t);
    memcpy(buffer, maze_update_buffer, len);
    maze_update_buffer_length = 0;
    return len;
  }

  return 0;
}

void maze_update(uint8_t x, uint8_t y, cell_t cell) {
  assert(ASSERT_MAZE + 0, x < MAZE_WIDTH);
  assert(ASSERT_MAZE + 1, y < MAZE_HEIGHT);
  assert(ASSERT_MAZE + 2, maze_update_buffer_length < 8);

  maze.cells[x][y] = cell;

  maze_update_buffer[maze_update_buffer_length++] = (maze_update_t){.x = x, .y = y, .cell = cell};
}
