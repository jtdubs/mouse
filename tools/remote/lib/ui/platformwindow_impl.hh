#pragma once

#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace app::ui {

class PlatformWindow : public ::ui::Window {
 public:
  PlatformWindow(app::remote::Remote *remote);

  virtual void Render() override;

 private:
  app::remote::Remote *remote_;
};

}  // namespace app::ui
