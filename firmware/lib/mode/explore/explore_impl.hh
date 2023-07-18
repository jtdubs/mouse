#pragma once

#include "explore.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/utils/dequeue.hh"

namespace explore {

// orientation_t represents a cardinal direction.
typedef enum : uint8_t { NORTH, EAST, SOUTH, WEST, INVALID } orientation_t;

// ENTRY_OFFSET is the offset into a cell at which the mouse's left and right sensors are
// pointed at the center of the side walls of that cell.  This is the point where the
// sensors will be sampled for the purpose of detecting walls.
constexpr float ENTRY_OFFSET = 16.0;  // mm

// dequeue_id_t is the ID of a dequeue of maze locations.
typedef enum : uint8_t {
  DEQUEUE_PATH,
  DEQUEUE_NEXT,
} dequeue_id_t;

// dequeue_update_t is an update event for a dequeue of maze locations.
#pragma pack(push, 1)
typedef struct {
  dequeue_id_t          dequeue_id : 6;
  dequeue::event_type_t event      : 2;
  maze::location_t      value;
} dequeue_update_t;
#pragma pack(pop)

// stop stops the mouse in the middle of the current cell.
void stop();

// face turns the mouse to face the given orientation, stopping first if needed.
void face(orientation_t orientation);

// advance moves the mouse forward one cell, updating the breadcrumb trail if needed.
void advance(maze::location_t loc, bool update_path);

// adjacent determines the orientation needed to drive between two adjancent cells.
orientation_t adjacent(maze::location_t a, maze::location_t b);

// classify senses the maze walls and updates the maze representation, adding any
// newly discovered cells to the "next" stack.
void classify(maze::location_t loc);

// update_location updates the mouse's knowledge of where it is within the cell based
// on the control module's position data.
void update_location();

// floodfill calculates the shortest path to the goal.
void floodfill();

}  // namespace explore