#include "ui.hh"

#include "commandwindow_impl.hh"
#include "controlwindow_impl.hh"
#include "imgui_internal.h"
#include "lib/ui/viewport.hh"
#include "mazewindow_impl.hh"
#include "platformwindow_impl.hh"
#include "toolbarwindow_impl.hh"

namespace mouse::app::ui {

void Layout(ImGuiID dock_id) {
  auto control_node  = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.36f, nullptr, &dock_id);
  auto maze_node     = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.30f, nullptr, &dock_id);
  auto platform_node = ImGui::DockBuilderSplitNode(control_node, ImGuiDir_Left, 0.50f, nullptr, &control_node);

  ImGui::DockBuilderDockWindow("Control", control_node);
  ImGui::DockBuilderDockWindow("Platform", platform_node);
  ImGui::DockBuilderDockWindow("Maze", maze_node);
  ImGui::DockBuilderDockWindow("Command", dock_id);
}

std::unique_ptr<mouse::ui::UI> New(app::remote::Remote* remote) {
  auto ui = std::make_unique<mouse::ui::UI>();

  ui->AddWindow(std::make_unique<mouse::ui::Viewport>(true, Layout));
  ui->AddWindow(std::make_unique<ToolbarWindow>(remote));
  ui->AddWindow(std::make_unique<CommandWindow>(remote));
  ui->AddWindow(std::make_unique<ControlWindow>(remote));
  ui->AddWindow(std::make_unique<MazeWindow>(remote));
  ui->AddWindow(std::make_unique<PlatformWindow>(remote));

  return ui;
}

}  // namespace mouse::app::ui
