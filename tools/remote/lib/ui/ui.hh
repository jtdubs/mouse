#pragma once

#include <memory>

#include "lib/ui/ui.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace mouse::app::ui {

std::unique_ptr<mouse::ui::UI> New(app::remote::Remote* remote);

}  // namespace mouse::app::ui
