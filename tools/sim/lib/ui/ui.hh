#pragma once

#include "lib/ui/ui.hh"
#include "tools/sim/lib/sim/sim.hh"

namespace app::ui {

std::unique_ptr<::ui::UI> New(app::sim::Sim* sim);

}  // namespace app::ui
