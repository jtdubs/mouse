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

#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/utils/dequeue.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::mode::explore {

// orientation_t represents a cardinal direction.
enum class Orientation : uint8_t { North, East, South, West, Invalid };

// dequeue_id_t is the ID of a dequeue of maze locations.
enum class DequeueID : uint8_t {
  Invalid = 0,
  Path    = 1,
  Next    = 2,
};

// DequeueUpdate is an update event for a dequeue of maze locations.
#pragma pack(push, 1)
struct DequeueUpdate {
  DequeueID      dequeue_id : 6;
  dequeue::Event event      : 2;
  maze::Location value;
};
#pragma pack(pop)

// Explore() is the entry point for the explore mode.
void Explore();

// Solve() is the entry point for solve mode.
void Solve();

// GetReport() is the report handler for the explore mode.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, DequeueID id) {
  switch (id) {
    case DequeueID::Invalid:
      o << "Invalid";
      break;
    case DequeueID::Path:
      o << "Path";
      break;
    case DequeueID::Next:
      o << "Next";
      break;
  }
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, Orientation orientation) {
  switch (orientation) {
    case Orientation::North:
      o << "North";
      break;
    case Orientation::East:
      o << "East";
      break;
    case Orientation::South:
      o << "South";
      break;
    case Orientation::West:
      o << "West";
      break;
    case Orientation::Invalid:
      o << "Invalid";
      break;
  }
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const DequeueUpdate update) {
  o << "explore::DequeueUpdate{" << std::endl;
  o << "  dequeue_id: " << update.dequeue_id << std::endl;
  o << "  event: " << update.event << std::endl;
  o << "  value: " << update.value << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace mouse::mode::explore
