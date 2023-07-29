#pragma once

#include "tools/remote/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class PlatformWindow : public Window {
 public:
  PlatformWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  remote::Remote *remote_;
};

}  // namespace ui
