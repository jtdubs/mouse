#pragma once

#include "maze.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace maze {

// Update is a single update to the maze.
#pragma pack(push, 1)
struct Update {
  Location location;
  Cell     cell;
};
#pragma pack(pop)

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "maze::State{" << std::endl;
  o << "  location: " << state->location << std::endl;
  o << "  cell: " << state->cell << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace maze
