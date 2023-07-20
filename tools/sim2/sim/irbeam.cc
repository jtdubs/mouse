#include "irbeam.hh"

namespace sim {

IRBeam::IRBeam(Position start, Position end, float distance, float wallAngle)
    : Start(start),  //
      End(end),
      Distance(distance),
      WallAngle(wallAngle) {}

}  // namespace sim
