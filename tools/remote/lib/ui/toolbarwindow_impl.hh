#pragma once

#include "firmware/lib/mode/remote/command.hh"
#include "lib/ui/toolbarwindow.hh"
#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace app::ui {

class ToolbarWindow : public ::ui::ToolbarWindow {
 public:
  ToolbarWindow(app::remote::Remote *remote);

 protected:
  virtual void RenderToolbar() override;

 private:
  app::remote::Remote       *remote_;
  ::remote::command::Command reset_command_;
  ::remote::command::Command explore_command_;
  ::remote::command::Command solve_command_;
};

}  // namespace app::ui
