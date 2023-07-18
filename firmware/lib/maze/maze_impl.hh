#pragma once

#include "maze.hh"

namespace maze {

// Update is a single update to the maze.
#pragma pack(push, 1)
struct Update {
  location_t location;
  cell_t     cell;
};
#pragma pack(pop)

}  // namespace maze
