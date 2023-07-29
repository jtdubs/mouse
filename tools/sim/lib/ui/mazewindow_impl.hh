#pragma once

#include <string>
#include <vector>

#include "lib/ui/window.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace app::ui {

class MazeWindow : public ::ui::Window {
 public:
  MazeWindow(sim::Sim *sim);

  virtual void Render() override;

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

}  // namespace app::ui
