#pragma once

#include <string>

namespace ui {

class Window {
 public:
  virtual void Render() = 0;

 protected:
  void Row(std::string label);
  void Icon(std::string name);
  bool IconButton(std::string name, std::string icon);
};

}  // namespace ui
