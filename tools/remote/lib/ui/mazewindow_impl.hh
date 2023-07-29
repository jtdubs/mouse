#pragma once

#include "lib/ui/window.hh"
#include "tools/remote/lib/remote/remote.hh"

namespace mouse::app::ui {

class MazeWindow : public mouse::ui::Window {
 public:
  MazeWindow(app::remote::Remote *remote);

  virtual void Render() override;

 private:
  app::remote::Remote *remote_;
};

}  // namespace mouse::app::ui
