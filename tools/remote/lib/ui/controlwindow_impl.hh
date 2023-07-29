#pragma once

#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace app::ui {

class ControlWindow : public ::ui::Window {
 public:
  ControlWindow(app::remote::Remote *remote);

  virtual void Render() override;

 private:
  app::remote::Remote *remote_;
};

}  // namespace app::ui
