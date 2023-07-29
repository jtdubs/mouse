#include <algorithm>
#include <format>

#include "imgui_internal.h"
#include "mazewindow_impl.hh"

namespace mouse::app::ui {

MazeWindow::MazeWindow(sim::Sim *sim)
    : Window(), sim_(sim), mazes_(), maze_index_(0), dragging_(false), drag_button_pos_(), drag_start_pos_() {}

void MazeWindow::Render() {
  if (!ImGui::Begin("Maze")) {
    ImGui::End();
    return;
  }

  if (mazes_.size() == 0) {
    mazes_ = sim::Maze::List();
  }

  ImGui::Text("Maze:");
  ImGui::SameLine(0, 10);
  if (ImGui::BeginCombo("##Maze", mazes_[maze_index_].c_str())) {
    for (size_t i = 0; std::string maze : mazes_) {
      bool selected = (i == maze_index_);
      if (ImGui::Selectable(maze.c_str(), selected)) {
        maze_index_ = i;
        sim_->SetMaze(maze);
      }
      if (selected) {
        ImGui::SetItemDefaultFocus();
      }
      i++;
    }
    ImGui::EndCombo();
  }

  RenderMaze();

  ImGui::End();
}

void MazeWindow::RenderMaze() {
  auto maze = sim_->GetMaze();
  if (maze == nullptr) {
    return;
  }

  int w, h;
  maze->GetSize(w, h);

  auto post_dim = ImVec2(w, h);
  auto grid_dim = post_dim - ImVec2(1, 1);

  auto canvas_size    = ImGui::GetContentRegionAvail();
  auto cell_size_px   = std::min(canvas_size.x / grid_dim.x, canvas_size.y / grid_dim.y);
  auto maze_size_px   = grid_dim * cell_size_px;
  auto mm_size_px     = cell_size_px / 180.0;
  auto margin_px      = (canvas_size - maze_size_px) / 2.0f;
  auto maze_origin_px = ImGui::GetCursorScreenPos() + ImVec2(0, canvas_size.y) + ImVec2(margin_px.x, -margin_px.y);

  ImGui::Dummy(canvas_size);
  if (ImGui::BeginPopupContextItem("Maze Context Menu")) {
    if (ImGui::Selectable("Start Position")) {
      sim_->SetMousePos(ImVec2(90.0, 36.0));
      sim_->SetMouseTheta(std::numbers::pi / 2.0f);
    }
    if (ImGui::Selectable("Calibration Position")) {
      sim_->SetMousePos(ImVec2(90.0, 164.0));
      sim_->SetMouseTheta(-std::numbers::pi / 2.0f);
    }
    if (ImGui::Selectable("Center in Cell")) {
      auto [x, y] = sim_->GetMousePos();
      sim_->SetMousePos(                                 //
          ImVec2((std::floor(x / 180.0) + 0.5) * 180.0,  //
                 (std::floor(y / 180.0) + 0.5) * 180.0));
    }
    ImGui::EndPopup();
  }

  auto draw_list = ImGui::GetWindowDrawList();

  // Draw the maze
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      auto post           = (*maze)(i, j);
      auto cell_origin_px = maze_origin_px + (ImVec2(i, -j) * cell_size_px);
      if (post.north) {
        draw_list->AddLine(cell_origin_px, cell_origin_px + ImVec2(0, -cell_size_px),
                           ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 2.0f);
      }
      if (post.east) {
        draw_list->AddLine(cell_origin_px, cell_origin_px + ImVec2(cell_size_px, 0),
                           ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 2.0f);
      }
    }
  }

  auto [x, y] = sim_->GetMousePos();
  auto theta  = sim_->GetMouseTheta();

  auto mouseXY = [&](ImVec2 v) -> ImVec2 {  //
    return maze_origin_px + (sim_->Project(v) * mm_size_px * ImVec2(1, -1));
  };

  std::vector<ImVec2> outline = {
      mouseXY(ImVec2(0, 45)),     //
      mouseXY(ImVec2(50, 45)),    //
      mouseXY(ImVec2(65, 30)),    //
      mouseXY(ImVec2(65, -30)),   //
      mouseXY(ImVec2(50, -45)),   //
      mouseXY(ImVec2(-35, -45)),  //
      mouseXY(ImVec2(-35, 45)),
  };

  // Calculate mouse bounds
  float min_x = INFINITY, min_y = INFINITY, max_x = -INFINITY, max_y = -INFINITY;
  for (auto v : outline) {
    min_x = std::min(min_x, v.x);
    min_y = std::min(min_y, v.y);
    max_x = std::max(max_x, v.x);
    max_y = std::max(max_y, v.y);
  }

  // Draw the mouse
  for (size_t i = 0; i < outline.size(); i++) {
    draw_list->AddLine(outline[i], outline[(i + 1) % outline.size()],
                       ImGui::ColorConvertFloat4ToU32(ImVec4(0.5, 1, 0.5, 1)), 2.0f);
  }
  draw_list->AddLine(mouseXY(ImVec2(0, 45)), mouseXY(ImVec2(0, -45)),
                     ImGui::ColorConvertFloat4ToU32(ImVec4(0.5, 1, 0.5, 1)), 2.0f);

  if (dragging_) {
    ImGui::SetCursorScreenPos(drag_button_pos_);
  } else {
    ImGui::SetCursorScreenPos(ImVec2(min_x, min_y));
  }
  ImGui::InvisibleButton("#Mouse", ImVec2(max_x - min_x, max_y - min_y));
  if (ImGui::IsItemHovered()) {
    auto wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0) {
      auto angle  = theta * 180.0 / std::numbers::pi;
      angle       = roundf(angle / 5.0) * 5.0;
      angle      += 5.0 * wheel;
      theta       = angle * std::numbers::pi / 180.0;
      sim_->SetMouseTheta(theta);
    }
  }
  if (ImGui::IsItemActive()) {
    if (!dragging_) {
      drag_start_pos_  = ImVec2(x, y);
      drag_button_pos_ = ImVec2(min_x, min_y);
      dragging_        = true;
    } else {
      auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 5.0) * (1.0 / mm_size_px);
      sim_->SetMousePos(drag_start_pos_ + ImVec2(delta.x, -delta.y));
    }
  } else {
    if (dragging_) {
      dragging_ = false;
      sim_->SetMousePos(                                    //
          ImVec2(std::clamp(x, 0.0f, grid_dim.x * 180.0f),  //
                 std::clamp(y, 0.0f, grid_dim.y * 180.0f)));
    }
  }

  for (auto hit : sim_->GetIRBeams()) {
    auto start_px = maze_origin_px + (hit.start * ImVec2(mm_size_px, -mm_size_px));
    auto end_px   = maze_origin_px + (hit.end * ImVec2(mm_size_px, -mm_size_px));
    draw_list->AddLine(start_px, end_px, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1)), 1.0f);
    draw_list->AddCircleFilled(end_px, 4, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1)));
  }
}

}  // namespace mouse::app::ui
