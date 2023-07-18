#pragma once

#include "maze.hh"

namespace maze {

// update_t is a single update to the maze.
#pragma pack(push, 1)
struct update_t {
  location_t location;
  cell_t     cell;
};
#pragma pack(pop)

}  // namespace maze
