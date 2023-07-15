#pragma once

#include <stdbool.h>

#include "control/config.h"

// cell_t represents a single cell in the maze.
#pragma pack(push, 1)
typedef struct {
  bool    wall_north : 1;
  bool    wall_east  : 1;
  bool    wall_south : 1;
  bool    wall_west  : 1;
  bool    visited    : 1;
  uint8_t padding    : 3;
  uint8_t distance   : 8;  // Distance from the center of the maze.
} maze_cell_t;
#pragma pack(pop)

// maze_t represents the entire maze at it's maximum allowed size.
typedef struct {
  maze_cell_t cells[256];
} maze_t;

// maze_location_t holds the coordinates of a cell in the maze.
// format: 0xXY, where X is the x coordinate and Y is the y coordinate.
typedef uint8_t maze_location_t;

// maze_location constructs a maze_location_t from x and y coordinates.
inline maze_location_t maze_location(uint8_t x, uint8_t y) {
  return (x << 4) | y;
}

// maze_x extracts the x coordinate from a maze_location_t.
inline uint8_t maze_x(maze_location_t loc) {
  return loc >> 4;
}

// maze_y extracts the y coordinate from a maze_location_t.
inline uint8_t maze_y(maze_location_t loc) {
  return loc & 0x0F;
}

// maze_init initializes the maze to it's default state.
void maze_init();

// maze_read returns the cell at the given location.
maze_cell_t maze_read(maze_location_t loc);

// maze_update updates the cell at the given location.
void maze_update(maze_location_t loc, maze_cell_t cell);

// maze_report generates a report of changes to the maze.
uint8_t maze_report(uint8_t *buffer, uint8_t len);

// maze_send triggers retransmission of the maze.
void maze_send();
