#include "toolbarwindow.hh"

#include <format>

#include "imgui_internal.h"

namespace mouse::ui {

ToolbarWindow::ToolbarWindow() : Window() {}

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

}  // namespace mouse::ui
