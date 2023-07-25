#pragma once

#include "position.hh"

namespace sim {

class IRBeam {
 public:
  IRBeam(Position start, Position end, float distance, float wallAngle);

 public:
  Position Start;
  Position End;
  float    Distance;
  float    WallAngle;
};

}  // namespace sim
