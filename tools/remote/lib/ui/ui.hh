#pragma once

#include <memory>

#include "lib/ui/ui.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace app::ui {

std::unique_ptr<::ui::UI> New(app::remote::Remote* remote);

}  // namespace app::ui
