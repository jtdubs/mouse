#pragma once

#include "explore.hh"

namespace explore {

// ENTRY_OFFSET is the offset into a cell at which the mouse's left and right sensors are
// pointed at the center of the side walls of that cell.  This is the point where the
// sensors will be sampled for the purpose of detecting walls.
constexpr float ENTRY_OFFSET = 16.0;  // mm

// stop stops the mouse in the middle of the current cell.
void Stop();

// face turns the mouse to face the given orientation, stopping first if needed.
void Face(Orientation orientation);

// advance moves the mouse forward one cell, updating the breadcrumb trail if needed.
void Advance(maze::Location loc, bool update_path);

// adjacent determines the orientation needed to drive between two adjancent cells.
Orientation Adjacent(maze::Location a, maze::Location b);

// classify senses the maze walls and updates the maze representation, adding any
// newly discovered cells to the "next" stack.
void Classify(maze::Location loc);

// UpdateLocation updates the mouse's knowledge of where it is within the cell based
// on the control module's position data.
void UpdateLocation();

// Floodfill calculates the shortest path to the goal.
void Floodfill();

}  // namespace explore
