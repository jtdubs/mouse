#include "position.hh"

namespace mouse::app::sim {

Position::operator ImVec2() const {
  return ImVec2(x, y);
}

Position::Position(std::initializer_list<float> l) : x(*l.begin()), y(*(l.begin() + 1)) {}
Position::Position(ImVec2 v) : x(v.x), y(v.y) {}

}  // namespace mouse::app::sim
