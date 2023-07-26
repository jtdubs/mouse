#pragma once

#include "tools/sim/lib/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class ControlsWindow : public Window {
 public:
  ControlsWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  sim::Sim *sim_;
};

}  // namespace ui
