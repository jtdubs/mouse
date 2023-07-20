#pragma once

#include <memory>
#include <vector>

#include "tools/sim2/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class UI {
 public:
  UI(sim::Sim* sim);

  void Run();

 private:
  std::vector<std::unique_ptr<Window>> windows_;
  sim::Sim*                            sim_;
};

}  // namespace ui
