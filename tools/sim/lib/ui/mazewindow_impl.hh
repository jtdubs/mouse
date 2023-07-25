#pragma once

#include <string>
#include <vector>

#include "tools/sim/lib/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class MazeWindow : public Window {
 public:
  MazeWindow(sim::Sim *sim);

  virtual void Render();

 private:
  void RenderMaze();

 private:
  sim::Sim                *sim_;
  std::vector<std::string> mazes_;
  size_t                   maze_index_;
  bool                     dragging_;
  ImVec2                   drag_button_pos_;
  ImVec2                   drag_start_pos_;
};

}  // namespace ui
