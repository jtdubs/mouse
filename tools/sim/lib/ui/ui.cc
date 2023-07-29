#include "ui.hh"

#include <GLFW/glfw3.h>
#include <stdio.h>

#include <format>
#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "controlswindow_impl.hh"
#include "imgui_internal.h"
#include "mazewindow_impl.hh"
#include "statuswindow_impl.hh"
#include "symbolswindow_impl.hh"
#include "textures_impl.hh"
#include "toolbarwindow_impl.hh"
#include "viewport_impl.hh"

namespace ui {

void glfw_error_callback(int error, const char* description) {
  std::cerr << std::format("GLFW error {}: {}", error, description) << std::endl;
}

UI::UI(sim::Sim* sim) : windows_(), sim_(sim) {
  windows_.push_back(std::unique_ptr<Window>(new Viewport()));
  windows_.push_back(std::unique_ptr<Window>(new ToolbarWindow(sim_)));
  windows_.push_back(std::unique_ptr<Window>(new StatusWindow(sim_)));
  windows_.push_back(std::unique_ptr<Window>(new ControlsWindow(sim_)));
  windows_.push_back(std::unique_ptr<Window>(new SymbolsWindow(sim_)));
  windows_.push_back(std::unique_ptr<Window>(new MazeWindow(sim_)));
}

void UI::Run() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    std::cerr << "glfwInit failed" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  auto window = glfwCreateWindow(1280, 720, "sim", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "glfwCreateWindow failed" << std::endl;
    return;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io        = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.Fonts->AddFontFromFileTTF("data/fonts/DroidSansMono.ttf", 24.0f);

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  LoadIcons();

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto& w : windows_) {
      w->Render();
    }

    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

}  // namespace ui
