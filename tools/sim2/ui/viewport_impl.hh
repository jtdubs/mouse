#pragma once

#include "window_impl.hh"

namespace ui {

class Viewport : public Window {
 public:
  Viewport();

  virtual void Render();
};

}  // namespace ui
