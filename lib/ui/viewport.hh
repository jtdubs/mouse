#pragma once

#include <functional>

#include "imgui_internal.h"
#include "window.hh"

namespace mouse::ui {

class Viewport : public Window {
 public:
  Viewport(bool toolbar, std::function<void(ImGuiID)> layout);

  virtual void Render() override;

 private:
  bool                         toolbar_;
  std::function<void(ImGuiID)> layout_;
};

}  // namespace mouse::ui
