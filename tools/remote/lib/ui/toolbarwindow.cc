#include <format>

#include "imgui_internal.h"
#include "lib/ui/textures.hh"
#include "toolbarwindow_impl.hh"

namespace mouse::app::ui {

ToolbarWindow::ToolbarWindow(app::remote::Remote *remote)  //
    : mouse::ui::ToolbarWindow(), remote_(remote), reset_command_(), explore_command_(), solve_command_() {
  reset_command_.type   = ::remote::command::Type::Reset;
  explore_command_.type = ::remote::command::Type::Explore;
  solve_command_.type   = ::remote::command::Type::Solve;
}

void ToolbarWindow::RenderToolbar() {
  bool connected = remote_->IsConnected();

  ImGui::BeginDisabled(!connected);

  if (IconButton("Reset", "restart")) {
    remote_->Send(reset_command_);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Reset Mouse");
  }

  ImGui::SameLine();

  if (IconButton("Explore", "navigation")) {
    remote_->Send(explore_command_);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Explore");
  }

  ImGui::SameLine();

  if (IconButton("Solve", "flag-checkered")) {
    remote_->Send(solve_command_);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Solve");
  }

  ImGui::EndDisabled();

  ImGui::SameLine(0, 20);

  if (IconButton("VCDRecord", "video-outline-black")) {
    // TODO
  }

  ImGui::SameLine();

  if (IconButton("VCDStop", "video-off-outline-black")) {
    // TODO
  }
}

}  // namespace mouse::app::ui
