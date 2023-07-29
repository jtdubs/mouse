#include <format>

#include "imgui_internal.h"
#include "statuswindow_impl.hh"
#include "textures_impl.hh"

namespace ui {

StatusWindow::StatusWindow(sim::Sim *sim) : Window(), sim_(sim) {}

void StatusWindow::Render() {
  if (!ImGui::Begin("Status")) {
    ImGui::End();
    return;
  }

  ImGui::BeginTable("##Status", 2);
  ImGui::TableSetupColumn("##StatusLabel", ImGuiTableColumnFlags_WidthFixed, 160.0f);
  ImGui::TableSetupColumn("##StatusControl", ImGuiTableColumnFlags_WidthStretch);

  auto leds = sim_->GetMouse().GetLEDs();

  Row("Status LEDs:");
  Icon(leds.GetLeft() ? "led-on-lightblue" : "led-off-white");
  ImGui::SameLine(0, 20);
  Icon(leds.GetOnboard() ? "led-on-organge" : "led-off-white");
  ImGui::SameLine(0, 20);
  Icon(leds.GetRight() ? "led-on-lightblue" : "led-off-white");

  Row("IR LEDs:");
  Icon(leds.GetIR() ? "led-on-red" : "led-off-white");
  ImGui::SameLine(0, 20);
  Icon(leds.GetIR() ? "led-on-red" : "led-off-white");
  ImGui::SameLine(0, 20);
  Icon(leds.GetIR() ? "led-on-red" : "led-off-white");

  Row("Sensors:");
  int sensors[3] = {
      static_cast<int>(sim_->GetMouse().GetLeftSensor().GetVoltage()),     //
      static_cast<int>(sim_->GetMouse().GetForwardSensor().GetVoltage()),  //
      static_cast<int>(sim_->GetMouse().GetRightSensor().GetVoltage())     //
  };
  ImGui::SliderInt3("##Sensors", sensors, 0, 5000);

  ImGui::EndTable();
  ImGui::End();
}

}  // namespace ui
