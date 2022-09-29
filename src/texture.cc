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

constexpr ScalarF SamplerLocation(ScalarF location, WrapMode mode) {
  // Section 3.7.6 "Texture Wrap Modes"
  // https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf
  switch (mode) {
    case WrapMode::kClamp:
      return glm::clamp(location, 0.0f, 1.0f);
    case WrapMode::kRepeat:
      return glm::fract(location);
    case WrapMode::kMirror: {
      const auto is_even = static_cast<int>(glm::floor(location)) % 2 == 0;
      const auto fract = glm::fract(location);
      return is_even ? fract : 1.0 - fract;
    }
  }
  return 0.0;
}

glm::vec4 Texture::Sample(glm::vec2 pos) const {
  return SampleClamped({SamplerLocation(pos.x, sampler_.wrap_mode_s),
                        SamplerLocation(pos.y, sampler_.wrap_mode_t)});
}

glm::vec4 Texture::SampleClamped(glm::vec2 pos) const {
  if (size_.x * size_.y <= 0) {
    return kColorBlack;
  }

  glm::ivec2 ipos = {pos.x * (size_.x - 1), pos.y * (size_.y - 1)};

  auto offset = size_.x * ipos.y + ipos.x;

  Color* icolor = reinterpret_cast<Color*>(decoded_) + offset;

  glm::vec4 color = *icolor;

  return {color.b, color.g, color.r, color.a};
}

}  // namespace sft
