#include "maze.h"

maze_t  maze;
uint8_t initial_report_row = 0;

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

uint8_t maze_report(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  if (initial_report_row == MAZE_HEIGHT) {
    return 0;
  }

  maze_update_t *updates = (maze_update_t *)buffer;

  for (int i = 0; i < MAZE_WIDTH; i++) {
    updates[i] = (maze_update_t){.x = i, .y = 0, .cell = maze.cells[i][initial_report_row]};
  }
  initial_report_row++;
  return MAZE_WIDTH * sizeof(maze_update_t);
}
