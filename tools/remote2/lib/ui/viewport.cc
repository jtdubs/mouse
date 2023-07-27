#include "imgui_internal.h"
#include "viewport_impl.hh"

namespace ui {

Viewport::Viewport() : Window() {}

void Viewport::Render() {
  static bool first_run = true;

  auto flags = ImGuiWindowFlags_NoTitleBar             //
             | ImGuiWindowFlags_NoDocking              //
             | ImGuiWindowFlags_NoResize               //
             | ImGuiWindowFlags_NoMove                 //
             | ImGuiWindowFlags_NoBringToFrontOnFocus  //
             | ImGuiWindowFlags_NoNavFocus             //
             | ImGuiWindowFlags_NoCollapse;

  auto vp = ImGui::GetMainViewport();

  auto viewport_pos  = vp->Pos + ImVec2(0, 48);
  auto viewport_size = vp->Size - ImVec2(0, 48);

  ImGui::SetNextWindowPos(viewport_pos);
  ImGui::SetNextWindowSize(viewport_size);
  ImGui::SetNextWindowViewport(vp->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport", nullptr, flags);
  ImGui::PopStyleVar(3);
  auto dock_id = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
  ImGui::End();

  if (first_run) {
    first_run = false;
    ImGui::DockBuilderRemoveNode(dock_id);
    ImGui::DockBuilderAddNode(dock_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dock_id, viewport_size);
    ImGui::DockBuilderFinish(dock_id);
  }
}

}  // namespace ui
