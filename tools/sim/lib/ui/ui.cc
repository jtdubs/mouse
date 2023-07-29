#include "ui.hh"

#include "controlswindow_impl.hh"
#include "imgui_internal.h"
#include "lib/ui/viewport.hh"
#include "mazewindow_impl.hh"
#include "statuswindow_impl.hh"
#include "symbolswindow_impl.hh"
#include "toolbarwindow_impl.hh"

namespace mouse::app::ui {

void Layout(ImGuiID dock_id) {
  auto symbol_node  = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.55f, nullptr, &dock_id);
  auto status_node  = ImGui::DockBuilderSplitNode(symbol_node, ImGuiDir_Down, 0.20f, nullptr, &symbol_node);
  auto control_node = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.20f, nullptr, &dock_id);

  ImGui::DockBuilderDockWindow("Symbols", symbol_node);
  ImGui::DockBuilderDockWindow("Status", status_node);
  ImGui::DockBuilderDockWindow("Controls", control_node);
  ImGui::DockBuilderDockWindow("Maze", dock_id);
}

std::unique_ptr<mouse::ui::UI> New(app::sim::Sim *sim) {
  auto ui = std::make_unique<mouse::ui::UI>();
  ui->AddWindow(std::make_unique<mouse::ui::Viewport>(true, Layout));
  ui->AddWindow(std::make_unique<ToolbarWindow>(sim));
  ui->AddWindow(std::make_unique<ControlsWindow>(sim));
  ui->AddWindow(std::make_unique<MazeWindow>(sim));
  ui->AddWindow(std::make_unique<StatusWindow>(sim));
  ui->AddWindow(std::make_unique<SymbolsWindow>(sim));
  return ui;
}

}  // namespace mouse::app::ui
