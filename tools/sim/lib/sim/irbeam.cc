#include "irbeam.hh"

namespace sim {

IRBeam::IRBeam(Position start, Position end, float distance, float wall_angle)
    : start(start),  //
      end(end),
      distance(distance),
      wall_angle(wall_angle) {}

}  // namespace sim
