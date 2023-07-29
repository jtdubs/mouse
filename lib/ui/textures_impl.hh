#pragma once

#include <GL/gl.h>

#include "textures.hh"

namespace mouse::ui {

void LoadIcons();

ImTextureID LoadIcon(std::string path);

}  // namespace mouse::ui
