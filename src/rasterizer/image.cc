/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace sft {

std::shared_ptr<Image> Image::Create(const char* file_path) {
  return std::shared_ptr<Image>(new Image(file_path));
}

std::shared_ptr<Image> Image::Create(std::shared_ptr<Mapping> mapping,
                                     glm::ivec2 size) {
  return std::shared_ptr<Image>(new Image(std::move(mapping), size));
}

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

  return SampleUV({SamplerLocation(pos.x, sampler_.wrap_mode_s),
                   SamplerLocation(pos.y, sampler_.wrap_mode_t)});
}

glm::vec4 Image::SampleUnitNearest(glm::vec2 uv) const {
  return SampleXY({
      glm::clamp<Scalar>(uv.x * size_.x, 0, size_.x - 1),
      glm::clamp<Scalar>(uv.y * size_.y, 0, size_.y - 1),
  });
}

glm::vec4 Image::SampleUnitLinear(glm::vec2 uv) const {
  ScalarF x = uv.x * size_.x;
  ScalarF y = uv.y * size_.y;

  ScalarF i0 = glm::floor(x - 0.5f);
  ScalarF j0 = glm::floor(y - 0.5f);

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

  ScalarF a = glm::fract(x - 0.5f);
  ScalarF b = glm::fract(y - 0.5f);

  glm::vec4 ti0j0 = SampleXY({i0, j0});
  glm::vec4 ti1j0 = SampleXY({i1, j0});
  glm::vec4 ti0j1 = SampleXY({i0, j1});
  glm::vec4 ti1j1 = SampleXY({i1, j1});

  return ((1 - a) * (1 - b) * ti0j0) +  //
         (a * (1 - b) * ti1j0) +        //
         ((1 - a) * b * ti0j1) +        //
         (a * b * ti1j1)                //
      ;
}

// From 3.7.7 Texture Minification
// https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf
glm::vec4 Image::SampleUV(glm::vec2 uv) const {
  switch (sampler_.min_mag_filter) {
    case Filter::kNearest:
      return SampleUnitNearest(uv);
    case Filter::kLinear:
      return SampleUnitLinear(uv);
  }
  return kColorBlack;
}

const uint8_t* Image::GetBuffer() const {
  return mapping_ ? mapping_->GetBuffer() : nullptr;
}

glm::vec4 Image::SampleXY(glm::ivec2 xy) const {
  xy = glm::clamp(xy, {0, 0}, {size_.x - 1, size_.y - 1});
  auto offset = size_.x * xy.y + xy.x;
  const Color* icolor = reinterpret_cast<const Color*>(GetBuffer()) + offset;
  return *icolor;
}

void Image::SetSampler(Sampler sampler) {
  sampler_ = std::move(sampler);
}

const Sampler& Image::GetSampler() const {
  return sampler_;
}

glm::ivec2 Image::GetSize() const {
  return size_;
}

bool Image::IsValid() const {
  return is_valid_;
}

}  // namespace sft
