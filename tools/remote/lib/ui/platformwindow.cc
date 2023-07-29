#include <format>
#include <sstream>

#include "firmware/platform/platform.hh"
#include "imgui_internal.h"
#include "platformwindow_impl.hh"

namespace ui {

PlatformWindow::PlatformWindow(remote::Remote *remote) : Window(), remote_(remote) {}

void PlatformWindow::Render() {
  if (!ImGui::Begin("Platform")) {
    ImGui::End();
    return;
  }

  auto report = remote_->GetPlatformReport();

  std::ostringstream oss;
  oss << report << std::endl;
  ImGui::TextUnformatted(oss.str().c_str());

  ImGui::End();
}

}  // namespace ui
