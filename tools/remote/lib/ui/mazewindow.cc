#include <format>
#include <sstream>

#include "firmware/platform/platform.hh"
#include "imgui_internal.h"
#include "mazewindow_impl.hh"

namespace mouse::app::ui {

MazeWindow::MazeWindow(app::remote::Remote *remote) : Window(), remote_(remote) {}

void MazeWindow::Render() {
  if (!ImGui::Begin("Maze")) {
    ImGui::End();
    return;
  }

  auto maze = remote_->GetMaze();

  auto grid_dim       = ImVec2(16, 16);
  auto canvas_size_px = ImGui::GetContentRegionAvail();
  auto cell_size_px   = std::min(canvas_size_px.x / grid_dim.x, canvas_size_px.y / grid_dim.y);
  auto maze_size_px   = grid_dim * cell_size_px;
  auto margin_px      = (canvas_size_px - maze_size_px) / 2.0f;
  auto maze_origin_px = ImGui::GetCursorScreenPos() + ImVec2(0, canvas_size_px.y) + ImVec2(margin_px.x, -margin_px.y);

  ImGui::Dummy(canvas_size_px);
  if (ImGui::BeginPopupContextItem("##MazePopup")) {
    if (ImGui::Selectable("Refresh")) {
      remote_->Send(::remote::command::Command{::remote::command::Type::SendMaze, {}});
    }
    ImGui::EndPopup();
  }

  auto draw_list = ImGui::GetWindowDrawList();

  draw_list->AddRectFilled(maze_origin_px, maze_origin_px + (ImVec2(16, -16) * cell_size_px),
                           IM_COL32(16, 16, 16, 255));

  for (ssize_t x = 0; x < grid_dim.x; x++) {
    for (ssize_t y = 0; y < grid_dim.y; y++) {
      auto loc            = maze::Location(x, y);
      auto cell           = maze.cells[loc];
      auto cell_origin_px = maze_origin_px + (ImVec2(x, -y) * cell_size_px);
      auto cell_corner_px = cell_origin_px + (ImVec2(1, -1) * cell_size_px);

      if (cell.visited) {
        draw_list->AddRectFilled(cell_origin_px, cell_corner_px, IM_COL32(50, 50, 50, 255));
      }

      if (cell.wall_north) {
        draw_list->AddLine(cell_origin_px + ImVec2(0, -cell_size_px),             //
                           cell_origin_px + ImVec2(cell_size_px, -cell_size_px),  //
                           IM_COL32(255, 255, 255, 255),                          //
                           2.0);
      }
      if (cell.wall_east) {
        draw_list->AddLine(cell_origin_px + ImVec2(cell_size_px, 0),              //
                           cell_origin_px + ImVec2(cell_size_px, -cell_size_px),  //
                           IM_COL32(255, 255, 255, 255),                          //
                           2.0);
      }
      if (cell.wall_south) {
        draw_list->AddLine(cell_origin_px + ImVec2(0, 0),             //
                           cell_origin_px + ImVec2(cell_size_px, 0),  //
                           IM_COL32(255, 255, 255, 255),              //
                           2.0);
      }
      if (cell.wall_west) {
        draw_list->AddLine(cell_origin_px + ImVec2(0, 0),              //
                           cell_origin_px + ImVec2(0, -cell_size_px),  //
                           IM_COL32(255, 255, 255, 255),               //
                           2.0);
      }
    }
  }

  for (auto next : remote_->GetNextDequeue()) {
    auto origin = maze_origin_px + ((ImVec2(next.X(), -next.Y()) + ImVec2(0.5, -0.5)) * cell_size_px);
    draw_list->AddCircleFilled(origin,              //
                               cell_size_px / 4.0,  //
                               IM_COL32(90, 30, 30, 255));
  }

  for (auto next : remote_->GetPathDequeue()) {
    auto origin = maze_origin_px + ((ImVec2(next.X(), -next.Y()) + ImVec2(0.5, -0.5)) * cell_size_px);
    draw_list->AddCircleFilled(origin,              //
                               cell_size_px / 8.0,  //
                               IM_COL32(0, 0, 0, 255));
  }

  ImGui::End();
}

}  // namespace mouse::app::ui
