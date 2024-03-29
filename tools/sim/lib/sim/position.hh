#pragma once

#include <initializer_list>

#include "imgui_internal.h"

namespace mouse::app::sim {

struct Position {
  float x;
  float y;

  operator ImVec2() const;

  Position(std::initializer_list<float>);
  Position(ImVec2);
};

}  // namespace mouse::app::sim
