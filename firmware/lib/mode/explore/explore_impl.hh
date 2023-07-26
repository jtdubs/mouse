#pragma once

#include "explore.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/utils/dequeue.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace explore {

// orientation_t represents a cardinal direction.
enum class Orientation : uint8_t { North, East, South, West, Invalid };

// ENTRY_OFFSET is the offset into a cell at which the mouse's left and right sensors are
// pointed at the center of the side walls of that cell.  This is the point where the
// sensors will be sampled for the purpose of detecting walls.
constexpr float ENTRY_OFFSET = 16.0;  // mm

// dequeue_id_t is the ID of a dequeue of maze locations.
enum class DequeueID : uint8_t {
  Path,
  Next,
};

// DequeueUpdate is an update event for a dequeue of maze locations.
#pragma pack(push, 1)
struct DequeueUpdate {
  DequeueID      dequeue_id : 6;
  dequeue::Event event      : 2;
  maze::Location value;
};
#pragma pack(pop)

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

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const DequeueID id) {
  switch (id) {
    case DequeueID::Path:
      o << "Path";
      break;
    case DequeueID::Next:
      o << "Next";
      break;
  }
}

std::ostream &operator<<(std::ostream &o, const DequeueUpdate *update) {
  o << "explore::DequeueUpdate{" << std::endl;
  o << "  dequeue_id: " << update->dequeue_id << std::endl;
  o << "  event: " << update->event << std::endl;
  o << "  value: " << update->value << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace explore
