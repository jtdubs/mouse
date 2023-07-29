#pragma once

#include "window.hh"

namespace ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow();

  virtual void Render() override;

 private:
  virtual void RenderToolbar() = 0;
};

}  // namespace ui
