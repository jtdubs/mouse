#include "viewport_impl.hh"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

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

  auto viewportPos  = vp->Pos + ImVec2(0, 48);
  auto viewportSize = vp->Size - ImVec2(0, 48);

  ImGui::SetNextWindowPos(viewportPos);
  ImGui::SetNextWindowSize(viewportSize);
  ImGui::SetNextWindowViewport(vp->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport", nullptr, flags);
  ImGui::PopStyleVar(3);
  auto dockID = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dockID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
  ImGui::End();

  if (first_run) {
    first_run = false;
    ImGui::DockBuilderRemoveNode(dockID);
    ImGui::DockBuilderAddNode(dockID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockID, viewportSize);
    auto symbolNode  = ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Right, 0.55f, nullptr, &dockID);
    auto statusNode  = ImGui::DockBuilderSplitNode(symbolNode, ImGuiDir_Down, 0.20f, nullptr, &symbolNode);
    auto controlNode = ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Down, 0.20f, nullptr, &dockID);
    ImGui::DockBuilderDockWindow("Symbols", symbolNode);
    ImGui::DockBuilderDockWindow("Status", statusNode);
    ImGui::DockBuilderDockWindow("Controls", controlNode);
    ImGui::DockBuilderDockWindow("Maze", dockID);
    ImGui::DockBuilderFinish(dockID);
  }
}

}  // namespace ui
