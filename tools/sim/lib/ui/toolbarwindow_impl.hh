#pragma once

#include "tools/sim/lib/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  void RenderToolbar();

 private:
  sim::Sim *sim_;
};

}  // namespace ui
