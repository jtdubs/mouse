#pragma once

#include "position.hh"

namespace mouse::app::sim {

struct IRBeam {
  Position start;
  Position end;
  float    distance;
  float    wall_angle;
};

}  // namespace mouse::app::sim
