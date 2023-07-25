#include "ui.hh"

#include <GLFW/glfw3.h>
#include <stdio.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "controlswindow_impl.hh"
#include "imgui.h"
#include "mazewindow_impl.hh"
#include "statuswindow_impl.hh"
#include "symbolswindow_impl.hh"
#include "textures_impl.hh"
#include "toolbarwindow_impl.hh"
#include "viewport_impl.hh"

namespace ui {

void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW error %d: %s\n", error, description);
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
    fprintf(stderr, "glfwInit failed\n");
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  auto window = glfwCreateWindow(1280, 720, "sim2", nullptr, nullptr);
  if (window == nullptr) {
    fprintf(stderr, "glfwCreateWindow failed\n");
    return;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io        = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.Fonts->AddFontFromFileTTF("tools/fonts/DroidSansMono.ttf", 24.0f);

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

    for (auto& window : windows_) {
      window->Render();
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
