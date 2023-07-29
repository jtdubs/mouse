#pragma once

#include "window.hh"

namespace mouse::ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow();

  virtual void Render() override;

 private:
  virtual void RenderToolbar() = 0;
};

}  // namespace mouse::ui
