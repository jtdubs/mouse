#include <math.h>

#include <format>

#include "mazewindow_impl.hh"
#include "textures_impl.hh"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace ui {

MazeWindow::MazeWindow(sim::Sim *sim)
    : Window(), sim_(sim), mazes_(), maze_index_(0), dragging_(false), drag_button_pos_(), drag_start_pos_() {}

void MazeWindow::Render() {
  if (!ImGui::Begin("Maze")) {
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
  auto cell_size_px   = fminf(canvas_size.x / grid_dim.x, canvas_size.y / grid_dim.y);
  auto maze_size_px   = grid_dim * cell_size_px;
  auto mm_size_px     = cell_size_px / 180.0;
  auto margin_px      = (canvas_size - maze_size_px) / 2.0f;
  auto maze_origin_px = ImGui::GetCursorScreenPos() + ImVec2(0, canvas_size.y) + ImVec2(margin_px.x, -margin_px.y);

  ImGui::Dummy(canvas_size);
  if (ImGui::BeginPopupContextItem("Maze Context Menu")) {
    if (ImGui::Selectable("Start Position")) {
      sim_->SetMousePos(ImVec2(90.0, 36.0));
      sim_->SetMouseTheta(M_PI_2);
    }
    if (ImGui::Selectable("Calibration Position")) {
      sim_->SetMousePos(ImVec2(90.0, 164.0));
      sim_->SetMouseTheta(-M_PI_2);
    }
    if (ImGui::Selectable("Center in Cell")) {
      auto [x, y] = sim_->GetMousePos();
      sim_->SetMousePos(                            //
          ImVec2((floor(x / 180.0) + 0.5) * 180.0,  //
                 (floor(y / 180.0) + 0.5) * 180.0));
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
  float minX = INFINITY, minY = INFINITY, maxX = -INFINITY, maxY = -INFINITY;
  for (auto v : outline) {
    minX = fminf(minX, v.x);
    minY = fminf(minY, v.y);
    maxX = fmaxf(maxX, v.x);
    maxY = fmaxf(maxY, v.y);
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
    ImGui::SetCursorScreenPos(ImVec2(minX, minY));
  }
  ImGui::InvisibleButton("#Mouse", ImVec2(maxX - minX, maxY - minY));
  if (ImGui::IsItemHovered()) {
    auto wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0) {
      auto angle  = theta * 180.0 / M_PI;
      angle       = roundf(angle / 5.0) * 5.0;
      angle      += 5.0 * wheel;
      theta       = angle * M_PI / 180.0;
      sim_->SetMouseTheta(theta);
    }
  }
  if (ImGui::IsItemActive()) {
    if (!dragging_) {
      drag_start_pos_  = ImVec2(x, y);
      drag_button_pos_ = ImVec2(minX, minY);
      dragging_        = true;
    } else {
      auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 5.0) * (1.0 / mm_size_px);
      sim_->SetMousePos(drag_start_pos_ + ImVec2(delta.x, -delta.y));
    }
  } else {
    if (dragging_) {
      dragging_ = false;
      sim_->SetMousePos(                                  //
          ImVec2(fminf(fmaxf(x, 0), grid_dim.x * 180.0),  //
                 fminf(fmaxf(y, 0), grid_dim.y * 180.0)));
    }
  }

  for (auto hit : sim_->GetIRBeams()) {
    auto start_px = maze_origin_px + (hit.Start * ImVec2(mm_size_px, -mm_size_px));
    auto end_px   = maze_origin_px + (hit.End * ImVec2(mm_size_px, -mm_size_px));
    draw_list->AddLine(start_px, end_px, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1)), 1.0f);
    draw_list->AddCircleFilled(end_px, 4, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1)));
  }
}

}  // namespace ui
