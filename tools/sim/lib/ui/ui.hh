#pragma once

#include "lib/ui/ui.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace mouse::app::ui {

std::unique_ptr<mouse::ui::UI> New(app::sim::Sim* sim);

}  // namespace mouse::app::ui
