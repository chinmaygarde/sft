#include "rasterizer.h"

namespace sft {

Rasterizer::Rasterizer(glm::ivec2 size)
    : color_buffer_(std::calloc(size.x * size.y * 4, sizeof(uint8_t))),
      depth_buffer_(std::calloc(size.x * size.y * 4, sizeof(uint8_t))),
      size_(size),
      viewport_(size) {}

Rasterizer::~Rasterizer() {
  std::free(depth_buffer_);
  std::free(color_buffer_);
}

void* Rasterizer::GetPixels() const {
  return color_buffer_;
}

void* Rasterizer::GetDepthPixels() const {
  return depth_buffer_;
}

glm::ivec2 Rasterizer::GetSize() const {
  return size_;
}

size_t Rasterizer::GetBytesPerPixel() const {
  return sizeof(uint32_t);
}

size_t Rasterizer::GetDepthBytesPerPixel() const {
  return sizeof(uint32_t);
}

void Rasterizer::SetDepthTestsEnabled(bool enabled) {
  depth_test_enabled_ = enabled;
}

constexpr bool IsClipped(glm::vec3 pos) {
  return pos.x < -1.0 || pos.x > 1.0 ||  //
         pos.y < -1.0 || pos.y > 1.0 ||  //
         pos.z < -1.0 || pos.z > 1.0;
}

constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
  return pos.x < 0 || pos.y < 0 || -pos.x >= size.x || pos.y >= size.y;
}

void Rasterizer::UpdateTexel(Texel texel) {
  if (IsOOB(texel.pos, size_)) {
    return;
  }

  const auto offset = size_.x * texel.pos.y + texel.pos.x;

  auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;
  auto depth_ptr = reinterpret_cast<uint32_t*>(depth_buffer_) + offset;

  auto new_depth = Color::Gray(texel.depth);

  if (depth_test_enabled_) {
    const auto old_depth = Color(depth_ptr[0]);
    if (new_depth.GetRed() > old_depth.GetRed()) {
      return;
    }
  }

  *color_ptr = texel.color;
  *depth_ptr = new_depth;
}

void Rasterizer::Clear(Color color) {
  Texel texel;
  texel.color = color;
  texel.depth = 0;

  for (size_t j = 0; j < size_.y; j++) {
    for (size_t i = 0; i < size_.x; i++) {
      texel.pos = {i, j};
      UpdateTexel(texel);
    }
  }
}

constexpr glm::ivec2 ToTexelPos(glm::vec3 nd_pos, glm::ivec2 viewport) {
  return {
      (viewport.x / 2.0) * (nd_pos.x + 1.0),  //
      (viewport.y / 2.0) * (nd_pos.y + 1.0),  //
  };
}

constexpr Rect GetBoundingBox(glm::ivec2 p1, glm::ivec2 p2, glm::ivec2 p3) {
  const auto min =
      glm::vec2{std::min({p1.x, p2.x, p3.x}), std::min({p1.y, p2.y, p3.y})};
  const auto max =
      glm::vec2{std::max({p1.x, p2.x, p3.x}), std::max({p1.y, p2.y, p3.y})};
  return Rect{{min.x, min.y}, {max.x - min.x, max.y - min.y}};
}

constexpr glm::vec3 GetBaryCentricCoordinates(glm::vec2 p,
                                              glm::vec2 a,
                                              glm::vec2 b,
                                              glm::vec2 c) {
  glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
  ScalarF one_over_den = 1.0f / (v0.x * v1.y - v1.x * v0.y);
  ScalarF v = (v2.x * v1.y - v1.x * v2.y) * one_over_den;
  ScalarF w = (v0.x * v2.y - v2.x * v0.y) * one_over_den;
  ScalarF u = 1.0f - v - w;
  return {u, v, w};
}

void Rasterizer::DrawTriangle(glm::vec3 ndc_p1,
                              glm::vec3 ndc_p2,
                              glm::vec3 ndc_p3,
                              Color color) {
  // This is different from OpenGL behavior. But close enough.
  if (IsClipped(ndc_p1) && IsClipped(ndc_p2) && IsClipped(ndc_p3)) {
    return;
  }

  const auto p1 = ToTexelPos(ndc_p1, viewport_);
  const auto p2 = ToTexelPos(ndc_p2, viewport_);
  const auto p3 = ToTexelPos(ndc_p3, viewport_);

  const auto bounding_box = GetBoundingBox(p1, p2, p3);
  for (auto y = 0; y < bounding_box.size.height; y++) {
    for (auto x = 0; x < bounding_box.size.width; x++) {
      const auto p =
          glm::vec2{x + bounding_box.origin.x, y + bounding_box.origin.y};
      const auto bary = GetBaryCentricCoordinates(p, p1, p2, p3);
      if (bary.x >= 0.0 && bary.y >= 0.0 && bary.z >= 0.0) {
        const auto bary_pos =
            (bary.x * ndc_p1 + bary.y * ndc_p2 + bary.z * ndc_p3);
        Texel texel;
        texel.pos = p;
        texel.depth = bary_pos.z;
        texel.color = color;
        UpdateTexel(texel);
      }
    }
  }
}

}  // namespace sft
