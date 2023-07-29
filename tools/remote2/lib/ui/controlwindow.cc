#include <format>
#include <sstream>

#include "controlwindow_impl.hh"
#include "firmware/platform/platform.hh"
#include "imgui_internal.h"

namespace ui {

ControlWindow::ControlWindow(remote::Remote *remote) : Window(), remote_(remote) {}

void ControlWindow::Render() {
  if (!ImGui::Begin("Control")) {
    ImGui::End();
    return;
  }

  auto report = remote_->GetControlReport();

  std::ostringstream oss;
  oss << report << std::endl;
  ImGui::TextUnformatted(oss.str().c_str());

  ImGui::End();
}

}  // namespace ui
