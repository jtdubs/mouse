#include <format>

#include "imgui_internal.h"
#include "textures_impl.hh"
#include "toolbarwindow_impl.hh"

namespace ui {

ToolbarWindow::ToolbarWindow(remote::Remote *remote) : Window(), remote_(remote) {}

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
    ImGui::End();
  } else {
    ImGui::PopStyleVar();
  }
}

void ToolbarWindow::RenderToolbar() {}

bool ToolbarWindow::Button(std::string name, std::string icon) {
  return ImGui::ImageButton(name.c_str(),        //
                            ui::Icon(icon),      //
                            ImVec2(24, 24),      //
                            ImVec2(0, 0),        //
                            ImVec2(1, 1),        //
                            ImVec4(0, 0, 0, 0),  //
                            ImVec4(1, 1, 1, 1));
}

}  // namespace ui
