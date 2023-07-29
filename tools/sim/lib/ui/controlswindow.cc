#include <format>

#include "controlswindow_impl.hh"
#include "imgui_internal.h"

namespace mouse::app::ui {

namespace {
const char *kFunctions[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
}

ControlsWindow::ControlsWindow(sim::Sim *sim) : Window(), sim_(sim) {}

void ControlsWindow::Render() {
  if (!ImGui::Begin("Controls")) {
    ImGui::End();
    return;
  }

  ImGui::BeginTable("Controls", 2);
  ImGui::TableSetupColumn("##ControlsLabel", ImGuiTableColumnFlags_WidthFixed, 160.0f);
  ImGui::TableSetupColumn("##ControlsControl", ImGuiTableColumnFlags_WidthStretch);

  Row("Voltage");
  int voltage = sim_->GetMouse().GetBattery().GetVoltage();
  if (ImGui::SliderInt("(mV)##Battery", &voltage, 0, 9000)) {
    sim_->GetMouse().GetBattery().SetVoltage(voltage);
  }

  Row("Function");
  int  function = sim_->GetMouse().GetSelector().GetFunction();
  bool button   = sim_->GetMouse().GetSelector().GetButton();
  if (ImGui::Combo("##Function", &function, kFunctions, 16)) {
    sim_->GetMouse().GetSelector().SetFunction(function);
  }
  ImGui::SameLine();
  if (ImGui::Checkbox("Button", &button)) {
    sim_->GetMouse().GetSelector().SetButton(button);
  }

  ImGui::EndTable();
  ImGui::End();
}

}  // namespace mouse::app::ui
