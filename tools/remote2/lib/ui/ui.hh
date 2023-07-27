#pragma once

#include <memory>
#include <vector>

#include "tools/remote2/lib/remote/remote.hh"
#include "window_impl.hh"

namespace ui {

class UI {
 public:
  UI(remote::Remote* remote);

  void Run();

 private:
  std::vector<std::unique_ptr<Window>> windows_;
  remote::Remote*                      remote_;
};

}  // namespace ui
