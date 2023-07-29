#pragma once

#include "position.hh"

namespace app::sim {

struct IRBeam {
  Position start;
  Position end;
  float    distance;
  float    wall_angle;
};

}  // namespace app::sim
