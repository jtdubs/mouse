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

namespace maze {

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

// Location holds the coordinates of a cell in the maze.
// format: 0xXY, where X is the x coordinate and Y is the y coordinate.
typedef uint8_t Location;

// location constructs a Location from x and y coordinates.
inline Location Loc(uint8_t x, uint8_t y) {
  return (x << 4) | y;
}

// x extracts the x coordinate from a Location.
inline uint8_t x(Location loc) {
  return loc >> 4;
}

// y extracts the y coordinate from a Location.
inline uint8_t y(Location loc) {
  return loc & 0x0F;
}

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

}  // namespace maze
