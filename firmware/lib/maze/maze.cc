#include <stdbool.h>
#include <string.h>
#include <util/atomic.h>

#include "firmware/lib/control/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "maze_impl.hh"

namespace maze {

namespace {
maze_t                          maze;
uint8_t                         report_row;
dequeue::dequeue<location_t, 6> updates;
}  // namespace

void init() {
  report_row = MAZE_HEIGHT;

  // Distances default to 0xFF, which is the maximum possible distance.
  for (uint16_t xy = 0; xy < (MAZE_WIDTH * MAZE_HEIGHT); xy++) {
    maze.cells[xy].distance = 0xFF;
  }

  // Bottom and top rows always have outer walls.
  for (uint8_t x = 0; x < MAZE_WIDTH; x++) {
    maze.cells[location(x, 0)].wall_south               = true;
    maze.cells[location(x, MAZE_HEIGHT - 1)].wall_north = true;
  }

  // Left and right columns always have outer walls.
  for (int y = 0; y < MAZE_HEIGHT; y++) {
    maze.cells[location(0, y)].wall_west              = true;
    maze.cells[location(MAZE_WIDTH - 1, y)].wall_east = true;
  }

  // The starting cell has been visited, and always has a wall to the east.
  maze.cells[location(0, 0)].visited   = true;
  maze.cells[location(0, 0)].wall_east = true;
}

void send() {
  report_row = 0;
}

uint8_t report(uint8_t *buffer, uint8_t len) {
  assert(assert::MAZE + 0, buffer != NULL);
  assert(assert::MAZE + 1, len >= (sizeof(update_t) * MAZE_WIDTH));

  auto *update_array = (update_t *)buffer;

  // if we have full rows to transmit, thensend the next one.
  if (report_row < MAZE_HEIGHT) {
    for (int i = 0; i < MAZE_WIDTH; i++) {
      update_array[i] = (update_t){
          .location = location(i, report_row),
          .cell     = maze.cells[location(i, report_row)],
      };
    }
    report_row++;
    return MAZE_WIDTH * sizeof(update_t);
  }

  // otherwise, send any pending updates.
  uint8_t report_len = 0;
  uint8_t i          = 0;
  while (!updates.empty()) {
    location_t loc     = updates.pop_front();
    update_array[i++]  = (update_t){.location = loc, .cell = maze.cells[loc]};
    report_len        += sizeof(update_t);
  }

  return report_len;
}

cell_t read(location_t loc) {
  assert(assert::MAZE + 2, x(loc) < MAZE_WIDTH);
  assert(assert::MAZE + 3, y(loc) < MAZE_HEIGHT);

  return maze.cells[loc];
}

void update(location_t loc, cell_t cell) {
  assert(assert::MAZE + 4, x(loc) < MAZE_WIDTH);
  assert(assert::MAZE + 5, y(loc) < MAZE_HEIGHT);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    maze.cells[loc] = cell;

    if (updates.full()) {
      send();
    } else {
      updates.push_back(loc);
    }
  }
}

}  // namespace maze
