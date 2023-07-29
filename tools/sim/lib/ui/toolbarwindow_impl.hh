#pragma once

#include "lib/ui/toolbarwindow.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace app::ui {

class ToolbarWindow : public ::ui::ToolbarWindow {
 public:
  ToolbarWindow(app::sim::Sim *sim);

 protected:
  virtual void RenderToolbar() override;

 private:
  app::sim::Sim *sim_;
};

}  // namespace app::ui
