#pragma once

#include "maze.hh"

namespace maze {

// update_t is a single update to the maze.
#pragma pack(push, 1)
typedef struct {
  location_t location;
  cell_t     cell;
} update_t;
#pragma pack(pop)

}
