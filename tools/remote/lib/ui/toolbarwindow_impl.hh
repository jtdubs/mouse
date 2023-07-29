#pragma once

#include "firmware/lib/mode/remote/command.hh"
#include "tools/remote/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  void RenderToolbar();

 private:
  remote::Remote          *remote_;
  remote::command::Command reset_command_;
  remote::command::Command explore_command_;
  remote::command::Command solve_command_;
};

}  // namespace ui
