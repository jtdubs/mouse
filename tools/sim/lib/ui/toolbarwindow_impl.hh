#pragma once

#include "lib/ui/toolbarwindow.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace mouse::app::ui {

class ToolbarWindow : public mouse::ui::ToolbarWindow {
 public:
  ToolbarWindow(app::sim::Sim *sim);

 protected:
  virtual void RenderToolbar() override;

 private:
  app::sim::Sim *sim_;
};

}  // namespace mouse::app::ui
