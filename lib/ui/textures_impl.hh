#pragma once

#include <GL/gl.h>

#include "textures.hh"

namespace ui {

void LoadIcons();

ImTextureID LoadIcon(std::string path);

}  // namespace ui
