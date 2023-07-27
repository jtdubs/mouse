#pragma once

#include <GL/gl.h>

#include <string>

#include "imgui_internal.h"

namespace ui {

void LoadIcons();

ImTextureID LoadIcon(std::string path);

ImTextureID Icon(std::string name);

}  // namespace ui
