#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace sft {

Image::Image(const char* path) {
  int width = 0;
  int height = 0;
  int channels_in_file = 0;

  auto* decoded = ::stbi_load(path, &width, &height, &channels_in_file, 4);

  if (!decoded) {
    std::cout << "Could not decode image at path: " << path;
    return;
  }

  mapping_ = std::make_shared<Mapping>(
      decoded, width * height * 4, [decoded]() { ::stbi_image_free(decoded); });
  size_ = {width, height};
}

Image::Image(std::shared_ptr<Mapping> mapping, glm::ivec2 size)
    : mapping_(std::move(mapping)), size_(size) {}

Image::~Image() = default;

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

glm::vec4 Image::Sample(glm::vec2 pos) const {
  if (size_.x * size_.y <= 0) {
    return kColorBlack;
  }

  return SampleUnit({SamplerLocation(pos.x, sampler_.wrap_mode_s),
                     SamplerLocation(pos.y, sampler_.wrap_mode_t)});
}

glm::vec4 Image::SampleUnitNearest(glm::vec2 st) const {
  return SampleUV({
      glm::clamp<Scalar>(st.x * size_.x, 0, size_.x - 1),
      glm::clamp<Scalar>(st.y * size_.y, 0, size_.y - 1),
  });
}

glm::vec4 Image::SampleUnitLinear(glm::vec2 st) const {
  ScalarF u = st.x * size_.x;
  ScalarF v = st.y * size_.y;

  ScalarF i0 = glm::floor(u - 0.5f);
  ScalarF j0 = glm::floor(v - 0.5f);

  if (sampler_.wrap_mode_s == WrapMode::kRepeat) {
    i0 = glm::mod(i0, static_cast<ScalarF>(size_.x));
  }

  if (sampler_.wrap_mode_t == WrapMode::kRepeat) {
    j0 = glm::mod(j0, static_cast<ScalarF>(size_.y));
  }

  ScalarF i1 = i0 + 1.0f;
  ScalarF j1 = j0 + 1.0f;

  if (sampler_.wrap_mode_s == WrapMode::kRepeat) {
    i1 = glm::mod(i1, static_cast<ScalarF>(size_.x));
  }

  if (sampler_.wrap_mode_t == WrapMode::kRepeat) {
    j1 = glm::mod(j1, static_cast<ScalarF>(size_.y));
  }

  ScalarF a = glm::fract(u - 0.5f);
  ScalarF b = glm::fract(v - 0.5f);

  glm::vec4 ti0j0 = SampleUV({i0, j0});
  glm::vec4 ti1j0 = SampleUV({i1, j0});
  glm::vec4 ti0j1 = SampleUV({i0, j1});
  glm::vec4 ti1j1 = SampleUV({i1, j1});

  return ((1 - a) * (1 - b) * ti0j0) +  //
         (a * (1 - b) * ti1j0) +        //
         ((1 - a) * b * ti0j1) +        //
         (a * b * ti1j1)                //
      ;
}

// From 3.7.7 Texture Minification
// https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf
glm::vec4 Image::SampleUnit(glm::vec2 pos) const {
  switch (sampler_.min_mag_filter) {
    case Filter::kNearest:
      return SampleUnitNearest(pos);
    case Filter::kLinear:
      return SampleUnitLinear(pos);
  }
  return kColorBlack;
}

const uint8_t* Image::GetBuffer() const {
  return mapping_ ? mapping_->GetBuffer() : nullptr;
}

glm::vec4 Image::SampleUV(glm::ivec2 uv) const {
  uv = glm::clamp(uv, {0, 0}, {size_.x - 1, size_.y - 1});
  auto offset = size_.x * uv.y + uv.x;
  const Color* icolor = reinterpret_cast<const Color*>(GetBuffer()) + offset;
  glm::vec4 color = *icolor;
  return {color.b, color.g, color.r, color.a};
}

}  // namespace sft
