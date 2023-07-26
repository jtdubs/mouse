#pragma once

#include <initializer_list>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace sim {

struct Position {
  float x;
  float y;

  operator ImVec2() const;

  Position(std::initializer_list<float>);
  Position(ImVec2);
};

}  // namespace sim
