#include "textures_impl.hh"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <map>
#include <memory>

namespace ui {

namespace {
std::map<std::string, ImTextureID> textures;
}

void LoadIcons() {
  std::filesystem::path folder = "data/icons";
  if (!std::filesystem::exists(folder)) {
    return;
  }

  for (const auto& entry : std::filesystem::directory_iterator(folder)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    auto path = entry.path().string();
    if (path.ends_with(".png")) {
      textures[entry.path().stem()] = LoadIcon(path);
    }
  }
}

ImTextureID LoadIcon(std::string path) {
  int  width, height, num_channels;
  auto data = std::unique_ptr<unsigned char, decltype(&stbi_image_free)>(
      stbi_load(path.c_str(), &width, &height, &num_channels, 4), &stbi_image_free);
  if (data == nullptr) {
    return 0;
  }

  GLint  current_texture;
  GLuint texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
  glBindTexture(GL_TEXTURE_2D, current_texture);

  return (void*)(intptr_t)(texture);
}

ImTextureID Icon(std::string name) {
  if (textures.count(name) > 0) {
    return textures[name];
  }

  return 0;
}

}  // namespace ui
