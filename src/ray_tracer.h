#pragma once

#include <vector>

#include "geom.h"
#include "macros.h"
#include "renderer.h"
#include "sphere.h"

namespace sft {

class RayTracer final : public Renderer {
 public:
  RayTracer(glm::ivec2 size)
      : size_(size),
        color_buffer_(std::calloc(size.x * size.y, sizeof(Color))) {
    SFT_ASSERT(color_buffer_ != nullptr);
  }

  ~RayTracer() { std::free(color_buffer_); }

  void UpdateTexel(Texel texel) {
    if (IsOOB(texel.pos, size_)) {
      return;
    }

    const auto offset = size_.x * texel.pos.y + texel.pos.x;

    auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;

    *color_ptr = texel.color;
  }

  constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
    return pos.x < 0 || pos.y < 0 || pos.x > size.x || pos.y > size.y;
  }

  void AddShape(Sphere sphere) { shapes_.emplace_back(std::move(sphere)); }

  void Render() {
    for (auto y = 0; y < size_.y; y++) {
      for (auto x = 0; x < size_.x; x++) {
        Texel texel;
        texel.pos = {x, y};
        texel.color =
            Color::FromComponentsF(x / static_cast<float>(size_.x),
                                   y / static_cast<float>(size_.y), 0.0, 1.0);
        UpdateTexel(texel);
      }
    }
  }

  // |Renderer|
  void* GetPixels() const override { return color_buffer_; }

  // |Renderer|
  void* GetDepthPixels() const override { return nullptr; }

  // |Renderer|
  glm::ivec2 GetSize() const override { return size_; }

  // |Renderer|
  size_t GetBytesPerPixel() const override { return sizeof(uint32_t); }

  // |Renderer|
  size_t GetDepthBytesPerPixel() const override { return 0u; }

 private:
  const glm::ivec2 size_;
  void* color_buffer_ = nullptr;
  std::vector<Sphere> shapes_;

  SFT_DISALLOW_COPY_AND_ASSIGN(RayTracer);
};

}  // namespace sft
