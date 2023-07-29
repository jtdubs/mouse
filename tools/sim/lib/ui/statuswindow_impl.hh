#pragma once

#include "lib/ui/window.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace mouse::app::ui {

class StatusWindow : public mouse::ui::Window {
 public:
  StatusWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  sim::Sim *sim_;
};

}  // namespace mouse::app::ui
