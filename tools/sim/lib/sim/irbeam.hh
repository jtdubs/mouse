#pragma once

#include "position.hh"

namespace sim {

struct IRBeam {
  Position start;
  Position end;
  float    distance;
  float    wall_angle;
};

}  // namespace sim
