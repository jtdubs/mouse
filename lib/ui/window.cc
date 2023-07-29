#include "window.hh"

#include "imgui_internal.h"
#include "textures_impl.hh"

namespace mouse::ui {

void Window::Row(std::string label) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::TextUnformatted(label.c_str());
  ImGui::TableSetColumnIndex(1);
}

void Window::Icon(std::string name) {
  ImGui::Image(ui::Icon(name), ImVec2(24, 24));
}

bool Window::IconButton(std::string name, std::string icon) {
  return ImGui::ImageButton(name.c_str(),        //
                            ui::Icon(icon),      //
                            ImVec2(24, 24),      //
                            ImVec2(0, 0),        //
                            ImVec2(1, 1),        //
                            ImVec4(0, 0, 0, 0),  //
                            ImVec4(1, 1, 1, 1));
}

bool Window::IconToggleButton(std::string label, std::string off_icon, std::string on_icon, bool &value) {
  bool result = IconButton(label, value ? on_icon : off_icon);
  if (result) {
    value = !value;
  }
  return result;
}

}  // namespace mouse::ui
