#pragma once

#include "firmware/lib/mode/remote/command.hh"
#include "tools/remote/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class CommandWindow : public Window {
 public:
  CommandWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  remote::Remote          *remote_;
  remote::command::Command tune_speed_command_;
  remote::command::Command tune_wall_command_;
  remote::command::Command tune_angle_command_;
  remote::command::Command led_command_;
  remote::command::Command ir_command_;
  remote::command::Command power_command_;
  remote::command::Command speed_command_;
  remote::command::Command linear_command_;
  remote::command::Command rotational_command_;
  remote::command::Command cal_command_;
  remote::command::Command execute_command_;
  bool                     link_power_;
  bool                     link_speed_;
};

}  // namespace ui
