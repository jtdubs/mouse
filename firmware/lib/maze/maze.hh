//
// System: maze
// Module: maze
//
// Purpose:
// - Hold the state of our knowledge of the maze.
// - Provide updates back to the remote when the maze is updated.
//
#pragma once

#include <stdbool.h>
#include <stddef.h>

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::maze {

// Cell represents a single cell in the maze.
#pragma pack(push, 1)
struct Cell {
  bool    wall_north : 1;
  bool    wall_east  : 1;
  bool    wall_south : 1;
  bool    wall_west  : 1;
  bool    visited    : 1;
  bool    padding    : 3;
  uint8_t distance;  // Distance from the center of the maze.
};
#pragma pack(pop)

// A bitfield is a 16x16 array of bits, organized as follows:
// - Each row is a 16-bit word.
// - Each cell in that row is a bit, in LSB order.
typedef uint16_t Bitfield[16];

// Maze represents the entire maze at it's maximum allowed size.
struct Maze {
  Bitfield walls_north;
  Bitfield walls_east;
  Bitfield walls_south;
  Bitfield walls_west;
  Bitfield visited;
  uint8_t  distances[256];
};

class Location {
 public:
  uint8_t x;
  uint8_t y;

 public:
  inline operator size_t() const {
    return (x << 4) | y;
  }

  inline Location operator+(Location o) const {
    return {static_cast<uint8_t>(x + o.x), static_cast<uint8_t>(y + o.y)};
  }

  inline void operator+=(Location o) {
    x += o.x;
    y += o.y;
  }

  inline Location operator-(Location o) const {
    return {static_cast<uint8_t>(x - o.x), static_cast<uint8_t>(y - o.y)};
  }
};

class PackedLocation {
 public:
  uint8_t xy;

 public:
  PackedLocation() = default;
  PackedLocation(const Location &loc) : xy((loc.x << 4) | loc.y) {}

  inline operator size_t() const {
    return xy;
  }

  inline operator Location() const {
    return {static_cast<uint8_t>(xy >> 4), static_cast<uint8_t>(xy & 0x0F)};
  }
};

// Init initializes the maze to it's default state.
void Init();

// Read maze data.
bool    WallNorth(Location loc);
bool    WallEast(Location loc);
bool    WallSouth(Location loc);
bool    WallWest(Location loc);
bool    Visited(Location loc);
uint8_t Distance(Location loc);

// Write maze data.
void SetWallNorth(Location loc);
void SetWallEast(Location loc);
void SetWallSouth(Location loc);
void SetWallWest(Location loc);
void SetVisited(Location loc);
void SetDistance(Location loc, uint8_t distance);
void Updated(Location loc);

// report generates a report of changes to the maze.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

// send triggers retransmission of the maze.
void Send();

// Update is a single update to the maze.
#pragma pack(push, 1)
struct Update {
  PackedLocation location;
  Cell           cell;
};
#pragma pack(pop)

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, Location location) {
  o << "maze::Location{" << static_cast<int>(location.x) << ", " << static_cast<int>(location.y) << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, PackedLocation location) {
  return o << static_cast<Location>(location);
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, Cell cell) {
  o << "maze::Cell{" << std::endl;
  o << "  wall_north: " << cell.wall_north << std::endl;
  o << "  wall_east: " << cell.wall_east << std::endl;
  o << "  wall_south: " << cell.wall_south << std::endl;
  o << "  wall_west: " << cell.wall_west << std::endl;
  o << "  visited: " << cell.visited << std::endl;
  o << "  distance: " << cell.distance << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Update update) {
  o << "maze::Update{" << std::endl;
  o << "  location: " << update.location << std::endl;
  o << "  cell: " << update.cell << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace mouse::maze
