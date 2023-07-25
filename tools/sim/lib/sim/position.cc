#include "position.hh"

namespace sim {

Position::Position(float x, float y) : x(x), y(y) {}

Position::Position(ImVec2 v) : x(v.x), y(v.y) {}

Position::operator ImVec2() const {
  return ImVec2(x, y);
}

}  // namespace sim
