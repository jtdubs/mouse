#pragma once

#include <GL/gl.h>

#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace ui {

void LoadIcons();

ImTextureID LoadIcon(std::string path);

ImTextureID Icon(std::string name);

}  // namespace ui
