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

#pragma pack(push, 1)
typedef struct {
  uint8_t x : 4;
  uint8_t y : 4;
  cell_t  cell;
} maze_update_t;
#pragma pack(pop)

extern maze_t maze;

void maze_init();

uint8_t maze_report(uint8_t *buffer, uint8_t len);
void    maze_update(uint8_t x, uint8_t y, cell_t cell);
