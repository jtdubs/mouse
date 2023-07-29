#pragma once

#include <memory>
#include <vector>

#include "window.hh"

namespace mouse::ui {

class UI {
 public:
  UI();

  void AddWindow(std::unique_ptr<Window> window);

  void Run();

 protected:
  virtual void Render();

 private:
  std::vector<std::unique_ptr<Window>> windows_;
};

}  // namespace mouse::ui
