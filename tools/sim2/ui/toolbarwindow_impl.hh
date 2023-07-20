#pragma once

#include "tools/sim2/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow(sim::Sim *sim);

  virtual void Render();

 private:
  void RenderToolbar();
  bool Button(std::string name, std::string icon);

 private:
  sim::Sim *sim_;
};

}  // namespace ui
