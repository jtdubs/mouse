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
  uint8_t padding    : 3;
  uint8_t distance   : 8;  // Distance from the center of the maze.
};
#pragma pack(pop)

// Maze represents the entire maze at it's maximum allowed size.
struct Maze {
  Cell cells[256];
};

class Location {
 private:
  uint8_t loc_;

 public:
  Location() : loc_(0) {}
  Location(uint8_t xy) : loc_(xy) {}
  Location(uint8_t x, uint8_t y) : loc_((x << 4) | y) {}
  Location(const Location &o) : loc_(o.loc_) {}

  inline uint8_t X() const {
    return loc_ >> 4;
  }

  inline uint8_t Y() const {
    return loc_ & 0x0F;
  }

  inline operator size_t() const {
    return loc_;
  }

  inline Location operator+(Location o) const {
    return Location(loc_ + o.loc_);
  }

  inline void operator+=(Location o) {
    loc_ += o.loc_;
  }

  inline Location operator-(Location o) const {
    return Location(loc_ - o.loc_);
  }

  inline void operator=(Location o) {
    loc_ = o.loc_;
  }
};

// Init initializes the maze to it's default state.
void Init();

// read returns the cell at the given location.
Cell Read(Location loc);

// update updates the cell at the given location.
void Write(Location loc, Cell cell);

// report generates a report of changes to the maze.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

// send triggers retransmission of the maze.
void Send();

// Update is a single update to the maze.
#pragma pack(push, 1)
struct Update {
  Location location;
  Cell     cell;
};
#pragma pack(pop)

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, Location location) {
  o << "maze::Location{" << static_cast<int>(location.X()) << ", " << static_cast<int>(location.Y()) << "}";
  return o;
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
