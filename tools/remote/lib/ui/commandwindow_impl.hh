#pragma once

#include "firmware/lib/mode/remote/command.hh"
#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace mouse::app::ui {

class CommandWindow : public mouse::ui::Window {
 public:
  CommandWindow(app::remote::Remote *remote);

  virtual void Render() override;

 private:
  app::remote::Remote           *remote_;
  mode::remote::command::Command tune_speed_command_;
  mode::remote::command::Command tune_wall_command_;
  mode::remote::command::Command tune_angle_command_;
  mode::remote::command::Command led_command_;
  mode::remote::command::Command ir_command_;
  mode::remote::command::Command power_command_;
  mode::remote::command::Command speed_command_;
  mode::remote::command::Command linear_command_;
  mode::remote::command::Command rotational_command_;
  mode::remote::command::Command cal_command_;
  mode::remote::command::Command execute_command_;
  bool                           link_power_;
  bool                           link_speed_;
};

}  // namespace mouse::app::ui
