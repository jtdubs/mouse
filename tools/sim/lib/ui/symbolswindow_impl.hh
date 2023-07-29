#pragma once

#include "imgui_internal.h"
#include "lib/ui/window.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace app::ui {

class SymbolsWindow : public ::ui::Window {
 public:
  SymbolsWindow(sim::Sim *sim);

  virtual void Render() override;

 private:
  sim::Sim                *sim_;
  ImGuiTextFilter          filter_;
  bool                     hex_;
  std::vector<std::string> names_;
};

}  // namespace app::ui
