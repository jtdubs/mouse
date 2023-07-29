#include <format>

#include "imgui_internal.h"
#include "textures_impl.hh"
#include "toolbarwindow_impl.hh"

namespace ui {

ToolbarWindow::ToolbarWindow(remote::Remote *remote)  //
    : Window(), remote_(remote), reset_command_(), explore_command_(), solve_command_() {
  reset_command_.type   = remote::command::Type::Reset;
  explore_command_.type = remote::command::Type::Explore;
  solve_command_.type   = remote::command::Type::Solve;
}

void ToolbarWindow::Render() {
  auto flags = ImGuiWindowFlags_NoDocking    //
             | ImGuiWindowFlags_NoTitleBar   //
             | ImGuiWindowFlags_NoResize     //
             | ImGuiWindowFlags_NoMove       //
             | ImGuiWindowFlags_NoScrollbar  //
             | ImGuiWindowFlags_NoSavedSettings;

  auto vp = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(vp->Pos);
  ImGui::SetNextWindowSize(ImVec2(vp->Size.x, 48));
  ImGui::SetNextWindowViewport(vp->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  if (ImGui::Begin("Toolbar", nullptr, flags)) {
    ImGui::PopStyleVar();
    RenderToolbar();
  } else {
    ImGui::PopStyleVar();
  }
  ImGui::End();
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

}  // namespace ui
