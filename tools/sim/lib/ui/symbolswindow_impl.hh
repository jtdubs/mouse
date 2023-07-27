#pragma once

#include "imgui_internal.h"
#include "tools/sim/lib/sim/sim.hh"
#include "window_impl.hh"

namespace ui {

class SymbolsWindow : public Window {
 public:
  SymbolsWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  sim::Sim                *sim_;
  ImGuiTextFilter          filter_;
  bool                     hex_;
  std::vector<std::string> names_;
};

}  // namespace ui
