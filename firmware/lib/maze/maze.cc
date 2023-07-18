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
dequeue::Dequeue<location_t, 6> updates;
}  // namespace

void Init() {
  report_row = kMazeHeight;

  // Distances default to 0xFF, which is the maximum possible distance.
  for (uint16_t xy = 0; xy < (kMazeWidth * kMazeHeight); xy++) {
    maze.cells[xy].distance = 0xFF;
  }

  // Bottom and top rows always have outer walls.
  for (uint8_t x = 0; x < kMazeWidth; x++) {
    maze.cells[location(x, 0)].wall_south               = true;
    maze.cells[location(x, kMazeHeight - 1)].wall_north = true;
  }

  // Left and right columns always have outer walls.
  for (int y = 0; y < kMazeHeight; y++) {
    maze.cells[location(0, y)].wall_west              = true;
    maze.cells[location(kMazeWidth - 1, y)].wall_east = true;
  }

  // The starting cell has been visited, and always has a wall to the east.
  maze.cells[location(0, 0)].visited   = true;
  maze.cells[location(0, 0)].wall_east = true;
}

void Send() {
  report_row = 0;
}

uint8_t GetReport(uint8_t *buffer, uint8_t len) {
  assert(assert::MAZE + 0, buffer != NULL);
  assert(assert::MAZE + 1, len >= (sizeof(Update) * kMazeWidth));

  auto *update_array = (Update *)buffer;

  // if we have full rows to transmit, then send the next one.
  if (report_row < kMazeHeight) {
    for (int i = 0; i < kMazeWidth; i++) {
      update_array[i] = (Update){
          .location = location(i, report_row),
          .cell     = maze.cells[location(i, report_row)],
      };
    }
    report_row++;
    return kMazeWidth * sizeof(Update);
  }

  // otherwise, send any pending updates.
  uint8_t report_len = 0;
  uint8_t i          = 0;
  while (!updates.Empty()) {
    location_t loc     = updates.PopFront();
    update_array[i++]  = (Update){.location = loc, .cell = maze.cells[loc]};
    report_len        += sizeof(Update);
  }

  return report_len;
}

cell_t Read(location_t loc) {
  assert(assert::MAZE + 2, x(loc) < kMazeWidth);
  assert(assert::MAZE + 3, y(loc) < kMazeHeight);

  return maze.cells[loc];
}

void Write(location_t loc, cell_t cell) {
  assert(assert::MAZE + 4, x(loc) < kMazeWidth);
  assert(assert::MAZE + 5, y(loc) < kMazeHeight);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    maze.cells[loc] = cell;

    if (updates.Full()) {
      Send();
    } else {
      updates.PushBack(loc);
    }
  }
}

}  // namespace maze
