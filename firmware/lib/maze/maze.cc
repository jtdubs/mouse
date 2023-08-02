#include <stdbool.h>
#include <string.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "maze_impl.hh"

namespace mouse::maze {

namespace {
Maze                          maze_;
uint8_t                       report_row;
dequeue::Dequeue<Location, 6> updates;
}  // namespace

void Init() {
  report_row = config::kMazeHeight;

  // Distances default to 0xFF, which is the maximum possible distance.
  for (uint16_t xy = 0; xy < 256; xy++) {
    maze_.distances[xy] = 0xFF;
  }

  // Bottom and top rows always have outer walls.
  maze_.walls_south[0]                       = 0b1111'1111'1111'1111;
  maze_.walls_north[config::kMazeHeight - 1] = 0b1111'1111'1111'1111;

  // Left and right columns always have outer walls.
  for (uint8_t y = 0; y < config::kMazeHeight; y++) {
    maze_.walls_west[y] = 0b0000'0000'0000'0001;
    maze_.walls_east[y] = 0b1000'0000'0000'0000;
  }

  // The starting cell has been visited, and always has a wall to the east.
  maze_.visited[0]     = 0b0000'0000'0000'0001;
  maze_.walls_east[0] |= 0b0000'0000'0000'0001;
}

void Send() {
  report_row = 0;
}

uint8_t GetReport(uint8_t *buffer, uint8_t len) {
  assert(assert::Module::Maze, 0, buffer != NULL);
  assert(assert::Module::Maze, 1, len >= (sizeof(Update) * config::kMazeWidth));

  auto *update_array = (Update *)buffer;

  // if we have full rows to transmit, then send the next one.
  if (report_row < config::kMazeHeight) {
    for (int i = 0; i < config::kMazeWidth; i++) {
      update_array[i] = (Update){
          .location = Location(i, report_row),
          .cell     = Read(Location(i, report_row)),
      };
    }
    report_row++;
    return config::kMazeWidth * sizeof(Update);
  }

  // otherwise, send any pending updates.
  uint8_t report_len = 0;
  uint8_t i          = 0;
  while (!updates.Empty()) {
    Location loc       = updates.PopFront();
    update_array[i++]  = (Update){.location = loc, .cell = Read(loc)};
    report_len        += sizeof(Update);
  }

  return report_len;
}

// TODO: Most read calls only access a single field, so faster to provide direct
// accessors for Visited(), Distance(), WallNorth(), etc.
Cell Read(Location loc) {
  assert(assert::Module::Maze, 2, loc.X() < config::kMazeWidth);
  assert(assert::Module::Maze, 3, loc.Y() < config::kMazeHeight);

  uint8_t x = loc.X();
  uint8_t y = loc.Y();

  return {
      .wall_north = (((maze_.walls_north[y] >> x) & 1) == 1),
      .wall_east  = (((maze_.walls_east[y] >> x) & 1) == 1),
      .wall_south = (((maze_.walls_south[y] >> x) & 1) == 1),
      .wall_west  = (((maze_.walls_west[y] >> x) & 1) == 1),
      .visited    = (((maze_.visited[y] >> x) & 1) == 1),
      .padding    = 0,
      .distance   = maze_.distances[loc],
  };
}

// TODO: Can flatten this into write calls for each field individually to avoid
// unpacking and repacking bits.
void Write(Location loc, Cell cell) {
  assert(assert::Module::Maze, 4, loc.X() < config::kMazeWidth);
  assert(assert::Module::Maze, 5, loc.Y() < config::kMazeHeight);

  uint8_t x = loc.X();
  uint8_t y = loc.Y();

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (cell.wall_north) {
      maze_.walls_north[y] |= (1 << x);
    } else {
      maze_.walls_north[y] &= ~(1 << x);
    }

    if (cell.wall_east) {
      maze_.walls_east[y] |= (1 << x);
    } else {
      maze_.walls_east[y] &= ~(1 << x);
    }

    if (cell.wall_south) {
      maze_.walls_south[y] |= (1 << x);
    } else {
      maze_.walls_south[y] &= ~(1 << x);
    }

    if (cell.wall_west) {
      maze_.walls_west[y] |= (1 << x);
    } else {
      maze_.walls_west[y] &= ~(1 << x);
    }

    if (cell.visited) {
      maze_.visited[y] |= (1 << x);
    } else {
      maze_.visited[y] &= ~(1 << x);
    }

    maze_.distances[loc] = cell.distance;

    if (updates.Full()) {
      Send();
    } else {
      updates.PushBack(loc);
    }
  }
}

}  // namespace mouse::maze
