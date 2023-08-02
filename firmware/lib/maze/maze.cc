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

Cell Read(Location loc) {
  return {
      .wall_north = WallNorth(loc),
      .wall_east  = WallEast(loc),
      .wall_south = WallSouth(loc),
      .wall_west  = WallWest(loc),
      .visited    = Visited(loc),
      .padding    = 0,
      .distance   = maze_.distances[loc],
  };
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

bool WallNorth(Location loc) {
  return (((maze_.walls_north[loc.Y()] >> loc.X()) & 1) == 1);
}

bool WallEast(Location loc) {
  return (((maze_.walls_east[loc.Y()] >> loc.X()) & 1) == 1);
}

bool WallSouth(Location loc) {
  return (((maze_.walls_south[loc.Y()] >> loc.X()) & 1) == 1);
}

bool WallWest(Location loc) {
  return (((maze_.walls_west[loc.Y()] >> loc.X()) & 1) == 1);
}

bool Visited(Location loc) {
  return (((maze_.visited[loc.Y()] >> loc.X()) & 1) == 1);
}

uint8_t Distance(Location loc) {
  return maze_.distances[loc];
}

void SetWallNorth(Location loc) {
  uint8_t x             = loc.X();
  uint8_t y             = loc.Y();
  maze_.walls_north[y] |= (1 << x);
}

void SetWallEast(Location loc) {
  uint8_t x            = loc.X();
  uint8_t y            = loc.Y();
  maze_.walls_east[y] |= (1 << x);
}

void SetWallSouth(Location loc) {
  uint8_t x             = loc.X();
  uint8_t y             = loc.Y();
  maze_.walls_south[y] |= (1 << x);
}

void SetWallWest(Location loc) {
  uint8_t x            = loc.X();
  uint8_t y            = loc.Y();
  maze_.walls_west[y] |= (1 << x);
}

void SetVisited(Location loc) {
  uint8_t x         = loc.X();
  uint8_t y         = loc.Y();
  maze_.visited[y] |= (1 << x);
}

void SetDistance(Location loc, uint8_t distance) {
  maze_.distances[loc] = distance;
}

void Updated(Location loc) {
  if (updates.Full()) {
    Send();
  } else {
    updates.PushBack(loc);
  }
}
}  // namespace mouse::maze
