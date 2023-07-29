#pragma once

#include "tools/remote2/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class ControlWindow : public Window {
 public:
  ControlWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  remote::Remote *remote_;
};

}  // namespace ui
