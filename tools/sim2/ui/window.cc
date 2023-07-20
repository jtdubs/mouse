#include "textures_impl.hh"
#include "window_impl.hh"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

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

}  // namespace ui
