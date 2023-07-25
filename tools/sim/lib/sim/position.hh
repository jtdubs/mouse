#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace sim {

class Position {
 public:
  Position(float x, float y);
  Position(ImVec2 v);

  operator ImVec2() const;

 public:
  float x;
  float y;
};

}  // namespace sim
