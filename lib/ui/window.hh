#pragma once

#include <string>

namespace mouse::ui {

class Window {
 public:
  virtual void Render() = 0;

 protected:
  void Row(std::string label);
  void Icon(std::string name);
  bool IconButton(std::string name, std::string icon);
  bool IconToggleButton(std::string label, std::string off_icon, std::string on_icon, bool &value);
};

}  // namespace mouse::ui
