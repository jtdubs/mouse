#pragma once

#include <stdbool.h>

#include "control/config.h"

#pragma pack(push, 1)
typedef struct {
  bool    wall_north : 1;
  bool    wall_east  : 1;
  bool    wall_south : 1;
  bool    wall_west  : 1;
  bool    visited    : 1;
  uint8_t padding    : 3;
  uint8_t distance   : 8;
} cell_t;
#pragma pack(pop)

typedef struct {
  cell_t cells[256];
} maze_t;

typedef uint8_t maze_location_t;

inline uint8_t maze_location(uint8_t x, uint8_t y) {
  return (x << 4) | y;
}

inline uint8_t maze_x(maze_location_t loc) {
  return loc >> 4;
}
inline uint8_t maze_y(maze_location_t loc) {
  return loc & 0x0F;
}

#pragma pack(push, 1)
typedef struct {
  maze_location_t location;
  cell_t          cell;
} maze_update_t;
#pragma pack(pop)

extern maze_t maze;

void maze_init();
void maze_send();

uint8_t maze_report(uint8_t *buffer, uint8_t len);
void    maze_update(maze_location_t loc, cell_t cell);
