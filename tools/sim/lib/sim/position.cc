#include "position.hh"

namespace sim {

Position::Position(float x, float y) : X(x), Y(y) {}

Position::Position(ImVec2 v) : X(v.x), Y(v.y) {}

Position::operator ImVec2() const {
  return ImVec2(X, Y);
}

}  // namespace sim
