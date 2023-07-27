#pragma once

#include "tools/remote2/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class ToolbarWindow : public Window {
 public:
  ToolbarWindow(remote::Remote *remote);

  virtual void Render() override;

 private:
  void RenderToolbar();
  bool Button(std::string name, std::string icon);

 private:
  remote::Remote *remote_;
};

}  // namespace ui
