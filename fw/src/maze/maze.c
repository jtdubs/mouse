#include "maze.h"

maze_t maze;

void maze_init() {
  for (int i = 0; i < MAZE_WIDTH; i++) {
    maze.cells[i][0].wall_south               = true;
    maze.cells[i][MAZE_HEIGHT - 1].wall_north = true;
  }
  for (int i = 0; i < MAZE_HEIGHT; i++) {
    maze.cells[0][i].wall_west              = true;
    maze.cells[MAZE_WIDTH - 1][i].wall_east = true;
  }
  maze.cells[0][0].wall_east = true;
}
