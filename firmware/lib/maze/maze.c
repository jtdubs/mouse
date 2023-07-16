#include "maze.h"

#include <string.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.h"
#include "firmware/lib/utils/dequeue.h"

// maze_update_t is a single update to the maze.
#pragma pack(push, 1)
typedef struct {
  maze_location_t location;
  maze_cell_t     cell;
} maze_update_t;
#pragma pack(pop)

static maze_t  maze;
static uint8_t maze_report_row;

DEFINE_DEQUEUE(maze_location_t, updates, 6);

void maze_init() {
  updates_init();

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

  // The starting cell has been visited, and always has a wall to the east.
  maze.cells[maze_location(0, 0)].visited   = true;
  maze.cells[maze_location(0, 0)].wall_east = true;
}

void maze_send() {
  maze_report_row = 0;
}

uint8_t maze_report(uint8_t *buffer, uint8_t len) {
  assert(ASSERT_MAZE + 0, buffer != NULL);
  assert(ASSERT_MAZE + 1, len >= (sizeof(maze_update_t) * MAZE_WIDTH));

  maze_update_t *updates = (maze_update_t *)buffer;

  // if we have full rows to transmit, thensend the next one.
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

  // otherwise, send any pending updates.
  uint8_t report_len = 0;
  uint8_t i          = 0;
  while (!updates_empty()) {
    maze_location_t loc  = updates_pop_front();
    updates[i++]         = (maze_update_t){.location = loc, .cell = maze.cells[loc]};
    report_len          += sizeof(maze_update_t);
  }

  return report_len;
}

maze_cell_t maze_read(maze_location_t loc) {
  assert(ASSERT_MAZE + 2, maze_x(loc) < MAZE_WIDTH);
  assert(ASSERT_MAZE + 3, maze_y(loc) < MAZE_HEIGHT);

  return maze.cells[loc];
}

void maze_update(maze_location_t loc, maze_cell_t cell) {
  assert(ASSERT_MAZE + 4, maze_x(loc) < MAZE_WIDTH);
  assert(ASSERT_MAZE + 5, maze_y(loc) < MAZE_HEIGHT);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    maze.cells[loc] = cell;

    if (updates_full()) {
      maze_send();
    } else {
      updates_push_back(loc);
    }
  }
}
