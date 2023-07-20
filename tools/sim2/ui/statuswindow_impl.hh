#pragma once

#include "tools/sim2/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class StatusWindow : public Window {
 public:
  StatusWindow(sim::Sim *sim);

  virtual void Render();

 private:
  sim::Sim *sim_;
};

}  // namespace ui
