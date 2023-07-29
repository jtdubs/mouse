#pragma once

#include "tools/remote2/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class MazeWindow : public Window {
 public:
  MazeWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  remote::Remote *remote_;
};

}  // namespace ui
