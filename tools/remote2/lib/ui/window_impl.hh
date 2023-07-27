#pragma once

#include <string>

namespace ui {

class Window {
 public:
  virtual void Render() = 0;

 protected:
  void Row(std::string label);
  void Icon(std::string name);
};

}  // namespace ui
