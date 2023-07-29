#pragma once

#include "firmware/lib/mode/remote/command.hh"
#include "lib/ui/toolbarwindow.hh"
#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace mouse::app::ui {

class ToolbarWindow : public mouse::ui::ToolbarWindow {
 public:
  ToolbarWindow(app::remote::Remote *remote);

 protected:
  virtual void RenderToolbar() override;

 private:
  app::remote::Remote  *remote_;
  mode::remote::Command reset_command_;
  mode::remote::Command explore_command_;
  mode::remote::Command solve_command_;
};

}  // namespace mouse::app::ui
