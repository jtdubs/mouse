#pragma once

#include "position.hh"

namespace sim {

class IRBeam {
 public:
  IRBeam(Position start, Position end, float distance, float wall_angle);

 public:
  Position start;
  Position end;
  float    distance;
  float    wall_angle;
};

}  // namespace sim
