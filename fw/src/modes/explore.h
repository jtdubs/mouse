//
// System: modes
// Module: explore
//
// Purpose:
// - The exploration mode of the mouse if responsible for traversing the maze,
//   updating the internal maze representation, and calculating the shortest
//   path to the goal.
// - Once complete, the mouse is returned to the starting location and exploration
//   mode is exited.
//
// Design:
// - The mouse holds two stacks of cell locations:
//   - A breadcrumb trail called "path" that is used to backtrack.
//   - A list of discovered but unvisited cells called "next".
// - The mouse starts with the starting square in "path" and the cell north of it in "next".
// - The algorithm proceeds as follows:
//     while (next is not empty) {
//       // skip any cells that were already visited after they were added to the stack.
//       while (next.top is visited) { next.pop() }
//       // if we are not next to the cell, backtrack, as we were definitely next to it at some point.
//       while (next.top is not adjacent to path.top) { backtrack }
//       drive to next.top
//       push adjacent, unvisited cells onto next
//     }
//
#pragma once

#include <stdint.h>

// explore_init initializes the explore mode.
void explore_init();

// explore() is the entry point for the explore mode.
void explore();

// explore_report() is the report handler for the explore mode.
uint8_t explore_report(uint8_t *buffer, uint8_t len);
