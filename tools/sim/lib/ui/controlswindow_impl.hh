#pragma once

#include "lib/ui/window.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace mouse::app::ui {

class ControlsWindow : public mouse::ui::Window {
 public:
  ControlsWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  sim::Sim *sim_;
};

}  // namespace mouse::app::ui
