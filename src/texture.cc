#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace sft {

Texture::Texture(const char* path) {
  int width = 0;
  int height = 0;
  int channels_in_file = 0;

  auto* decoded = ::stbi_load(path, &width, &height, &channels_in_file, 4);

  if (!decoded) {
    std::cout << "Could not decode image at path: " << path;
    return;
  }

  decoded_ = decoded;
  size_ = {width, height};
}

Texture::~Texture() {
  if (decoded_ != nullptr) {
    ::stbi_image_free(decoded_);
  }
}

glm::vec4 Texture::Sample(glm::vec2 pos) const {
  if (size_.x * size_.y <= 0) {
    return kColorFirebrick;
  }

  pos = {glm::clamp<ScalarF>(pos.x, 0.0, 1.0),
         glm::clamp<ScalarF>(pos.y, 0.0, 1.0)};

  glm::ivec2 ipos = {pos.x * (size_.x - 1), pos.y * (size_.y - 1)};

  auto offset = size_.x * ipos.y + ipos.x;

  Color* icolor = reinterpret_cast<Color*>(decoded_) + offset;

  glm::vec4 color = *icolor;

  return {color.b, color.g, color.r, color.a};
}

}  // namespace sft
