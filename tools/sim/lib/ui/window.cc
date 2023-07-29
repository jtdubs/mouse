#include "imgui_internal.h"
#include "textures_impl.hh"
#include "window_impl.hh"

namespace ui {

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

}  // namespace ui
