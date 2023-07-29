#include <format>
#include <sstream>

#include "commandwindow_impl.hh"
#include "firmware/lib/control/config.hh"
#include "firmware/platform/platform.hh"
#include "imgui_internal.h"

namespace ui {

CommandWindow::CommandWindow(remote::Remote *remote)  //
    : Window(),
      remote_(remote),
      tune_speed_command_(),
      tune_wall_command_(),
      tune_angle_command_(),
      led_command_(),
      ir_command_(),
      power_command_(),
      speed_command_(),
      linear_command_(),
      rotational_command_(),
      cal_command_(),
      execute_command_(),
      link_power_(true),
      link_speed_(true) {
  tune_speed_command_.type           = remote::command::Type::TunePID;
  tune_speed_command_.data.pid.id    = remote::command::PidID::Speed;
  tune_speed_command_.data.pid.kp    = kSpeedKp;
  tune_speed_command_.data.pid.ki    = kSpeedKi;
  tune_speed_command_.data.pid.kd    = kSpeedKd;
  tune_speed_command_.data.pid.alpha = kSpeedAlpha;

  tune_wall_command_.type           = remote::command::Type::TunePID;
  tune_wall_command_.data.pid.id    = remote::command::PidID::Wall;
  tune_wall_command_.data.pid.kp    = kWallKp;
  tune_wall_command_.data.pid.ki    = kWallKi;
  tune_wall_command_.data.pid.kd    = kWallKd;
  tune_wall_command_.data.pid.alpha = kWallAlpha;

  tune_angle_command_.type           = remote::command::Type::TunePID;
  tune_angle_command_.data.pid.id    = remote::command::PidID::Angle;
  tune_angle_command_.data.pid.kp    = kAngleKp;
  tune_angle_command_.data.pid.ki    = kAngleKi;
  tune_angle_command_.data.pid.kd    = kAngleKd;
  tune_angle_command_.data.pid.alpha = kAngleAlpha;

  led_command_.type                        = remote::command::Type::PlanEnqueue;
  led_command_.data.plan.type              = plan::Type::LEDs;
  led_command_.data.plan.state             = plan::State::Scheduled;
  led_command_.data.plan.data.leds.left    = false;
  led_command_.data.plan.data.leds.right   = false;
  led_command_.data.plan.data.leds.onboard = false;

  ir_command_.type                 = remote::command::Type::PlanEnqueue;
  ir_command_.data.plan.type       = plan::Type::IR;
  ir_command_.data.plan.state      = plan::State::Scheduled;
  ir_command_.data.plan.data.ir.on = false;

  power_command_.type                       = remote::command::Type::PlanEnqueue;
  power_command_.data.plan.type             = plan::Type::FixedPower;
  power_command_.data.plan.state            = plan::State::Scheduled;
  power_command_.data.plan.data.power.left  = 0.0;
  power_command_.data.plan.data.power.right = 0.0;

  speed_command_.type                       = remote::command::Type::PlanEnqueue;
  speed_command_.data.plan.type             = plan::Type::FixedSpeed;
  speed_command_.data.plan.state            = plan::State::Scheduled;
  speed_command_.data.plan.data.speed.left  = 0.0;
  speed_command_.data.plan.data.speed.right = 0.0;

  linear_command_.type                           = remote::command::Type::PlanEnqueue;
  linear_command_.data.plan.type                 = plan::Type::LinearMotion;
  linear_command_.data.plan.state                = plan::State::Scheduled;
  linear_command_.data.plan.data.linear.position = 180.0;
  linear_command_.data.plan.data.linear.stop     = true;

  rotational_command_.type                              = remote::command::Type::PlanEnqueue;
  rotational_command_.data.plan.type                    = plan::Type::RotationalMotion;
  rotational_command_.data.plan.state                   = plan::State::Scheduled;
  rotational_command_.data.plan.data.rotational.d_theta = std::numbers::pi / 2.0f;

  cal_command_.type            = remote::command::Type::PlanEnqueue;
  cal_command_.data.plan.type  = plan::Type::SensorCal;
  cal_command_.data.plan.state = plan::State::Scheduled;

  execute_command_.type = remote::command::Type::PlanExecute;
}

void CommandWindow::Render() {
  if (!ImGui::Begin("Command")) {
    ImGui::End();
    return;
  }

  ImGui::BeginDisabled(!remote_->IsConnected());

  ImGui::BeginTable("##Controls", 3);
  ImGui::TableSetupColumn("##ControlsLabel", ImGuiTableColumnFlags_WidthFixed, 160.0f);
  ImGui::TableSetupColumn("##ControlsControl", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableSetupColumn("##ControlsButton", ImGuiTableColumnFlags_WidthFixed, 48.0f);

  Row("Speed PID:");
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 48.0f);
  ImGui::InputFloat4("##SpeedKp", &tune_speed_command_.data.pid.kp, "%.4f");
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##SpeedPID", "play-black")) {
    remote_->Send(tune_speed_command_);
  }

  Row("Wall PID:");
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 48.0f);
  ImGui::InputFloat4("##SpeedKp", &tune_wall_command_.data.pid.kp, "%.4f");
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##WallPID", "play-black")) {
    remote_->Send(tune_wall_command_);
  }

  Row("Angle PID:");
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 48.0f);
  ImGui::InputFloat4("##SpeedKp", &tune_angle_command_.data.pid.kp, "%.4f");
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##AnglePID", "play-black")) {
    remote_->Send(tune_angle_command_);
  }

  ImGui::TableNextRow();
  for (size_t i = 0; i < 3; i++) {
    ImGui::TableSetColumnIndex(i);
    ImGui::Separator();
  }

  Row("Status LEDs:");
  IconToggleButton("##LeftLED", "led-off-white", "led-on-lightblue", led_command_.data.plan.data.leds.left);
  ImGui::SameLine(0, 20);
  IconToggleButton("##OnboardLED", "led-off-white", "led-on-orange", led_command_.data.plan.data.leds.onboard);
  ImGui::SameLine(0, 20);
  IconToggleButton("##RightLED", "led-off-white", "led-on-lightblue", led_command_.data.plan.data.leds.right);
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##LEDPlan", "plus-thick")) {
    remote_->Send(led_command_);
  }

  Row("IR LEDs:");
  IconToggleButton("##IR1", "led-off-white", "led-on-red", ir_command_.data.plan.data.ir.on);
  ImGui::SameLine(0, 20);
  IconToggleButton("##IR2", "led-off-white", "led-on-red", ir_command_.data.plan.data.ir.on);
  ImGui::SameLine(0, 20);
  IconToggleButton("##IR3", "led-off-white", "led-on-red", ir_command_.data.plan.data.ir.on);
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##IRPlan", "plus-thick")) {
    remote_->Send(ir_command_);
  }

  Row("Power:");
  IconToggleButton("##LinkPower", "link", "link-off", link_power_);
  ImGui::SameLine(0, 20);
  if (link_power_) {
    int left = power_command_.data.plan.data.power.left;
    if (ImGui::InputInt("##Power", &left)) {
      power_command_.data.plan.data.power.left  = left;
      power_command_.data.plan.data.power.right = left;
    }
  } else {
    int power[2] = {power_command_.data.plan.data.power.left, power_command_.data.plan.data.power.right};
    if (ImGui::InputInt2("##Power", power)) {
      power_command_.data.plan.data.power.left  = power[0];
      power_command_.data.plan.data.power.right = power[1];
    }
  }
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##PowerPlan", "plus-thick")) {
    remote_->Send(power_command_);
  }

  Row("Speed:");
  IconToggleButton("##LinkSpeed", "link", "link-off", link_speed_);
  ImGui::SameLine(0, 20);
  if (link_speed_) {
    if (ImGui::InputFloat("##Speed", &speed_command_.data.plan.data.speed.left)) {
      speed_command_.data.plan.data.speed.right = speed_command_.data.plan.data.speed.left;
    }
  } else {
    ImGui::InputFloat2("##Speed", &speed_command_.data.plan.data.speed.left);
  }
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##SpeedPlan", "plus-thick")) {
    remote_->Send(speed_command_);
  }

  Row("Linear:");
  ImGui::InputFloat("mm", &linear_command_.data.plan.data.linear.position);
  ImGui::SameLine();
  ImGui::Checkbox("stop", &linear_command_.data.plan.data.linear.stop);
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##LinearPlan", "plus-thick")) {
    remote_->Send(linear_command_);
  }

  Row("Rotational:");
  ImGui::InputFloat("°", &rotational_command_.data.plan.data.rotational.d_theta);
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##RotationalPlan", "plus-thick")) {
    remote_->Send(rotational_command_);
  }

  Row("Calibrate:");
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##CalPlan", "plus-thick")) {
    remote_->Send(cal_command_);
  }

  Row("");
  ImGui::TableSetColumnIndex(2);
  if (IconButton("##Execute", "play-black")) {
    remote_->Send(execute_command_);
  }

  ImGui::EndTable();
  ImGui::EndDisabled();
  ImGui::End();
}

}  // namespace ui
