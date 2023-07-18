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

// cell_t represents a single cell in the maze.
#pragma pack(push, 1)
struct cell_t {
  bool    wall_north : 1;
  bool    wall_east  : 1;
  bool    wall_south : 1;
  bool    wall_west  : 1;
  bool    visited    : 1;
  uint8_t padding    : 3;
  uint8_t distance   : 8;  // Distance from the center of the maze.
};
#pragma pack(pop)

// maze_t represents the entire maze at it's maximum allowed size.
struct maze_t {
  cell_t cells[256];
};

// location_t holds the coordinates of a cell in the maze.
// format: 0xXY, where X is the x coordinate and Y is the y coordinate.
typedef uint8_t location_t;

// location constructs a location_t from x and y coordinates.
inline location_t location(uint8_t x, uint8_t y) {
  return (x << 4) | y;
}

// x extracts the x coordinate from a location_t.
inline uint8_t x(location_t loc) {
  return loc >> 4;
}

// y extracts the y coordinate from a location_t.
inline uint8_t y(location_t loc) {
  return loc & 0x0F;
}

// Init initializes the maze to it's default state.
void Init();

// read returns the cell at the given location.
cell_t Read(location_t loc);

// update updates the cell at the given location.
void Write(location_t loc, cell_t cell);

// report generates a report of changes to the maze.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

// send triggers retransmission of the maze.
void Send();

}  // namespace maze
