#pragma once

#include <stdbool.h>

#include "control/config.h"

#pragma pack(push, 1)
typedef struct {
  bool wall_north : 1;
  bool wall_east  : 1;
  bool wall_south : 1;
  bool wall_west  : 1;
} cell_t;
#pragma pack(pop)

typedef struct {
  cell_t cells[MAZE_WIDTH][MAZE_HEIGHT];
} maze_t;

extern maze_t maze;

void maze_init();
