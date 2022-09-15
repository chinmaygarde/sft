#include "rasterizer.h"
#include <strings.h>
#include "macros.h"

namespace sft {

Rasterizer::Rasterizer(glm::ivec2 size)
    : color_buffer_(std::calloc(size.x * size.y, sizeof(Color))),
      depth_buffer_(std::calloc(size.x * size.y, sizeof(Color))),
      size_(size) {}

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

constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
  return pos.x < 0 || pos.y < 0 || pos.x > size.x || pos.y > size.y;
}

void Rasterizer::UpdateTexel(Texel texel) {
  if (IsOOB(texel.pos, size_)) {
    return;
  }

  const auto offset = size_.x * texel.pos.y + texel.pos.x;

  auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;
  auto depth_ptr = reinterpret_cast<uint32_t*>(depth_buffer_) + offset;

  auto new_depth = Color::Gray(texel.depth);

  if (pipeline_->depth_test_enabled) {
    const auto old_depth = Color(depth_ptr[0]);
    if (new_depth.GetRed() > old_depth.GetRed()) {
      return;
    }
  }

  *color_ptr = pipeline_->Blend(texel.color, *color_ptr);
  *depth_ptr = new_depth;
}

void Rasterizer::Clear(Color color) {
  memset_pattern4(color_buffer_, &color, size_.x * size_.y * sizeof(Color));
  bzero(depth_buffer_, size_.x * size_.y * sizeof(color));
}

constexpr glm::vec2 ToTexelPos(glm::vec3 nd_pos, const glm::ivec2& viewport) {
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
  auto one_over_den = 1.0 / (v0.x * v1.y - v1.x * v0.y);
  auto v = (v2.x * v1.y - v1.x * v2.y) * one_over_den;
  auto w = (v0.x * v2.y - v2.x * v0.y) * one_over_den;
  auto u = 1.0 - v - w;
  return {u, v, w};
}

void Rasterizer::DrawTriangle(glm::vec3 ndc_p1,
                              glm::vec3 ndc_p2,
                              glm::vec3 ndc_p3) {
  SFT_ASSERT(pipeline_ && "Must have pipeline");
  auto viewport = pipeline_->viewport;

  ndc_p1 = pipeline_->shader->ProcessVertex(ndc_p1, 0u);
  ndc_p2 = pipeline_->shader->ProcessVertex(ndc_p2, 1u);
  ndc_p3 = pipeline_->shader->ProcessVertex(ndc_p3, 2u);

  const auto p1 = ToTexelPos(ndc_p1, viewport);
  const auto p2 = ToTexelPos(ndc_p2, viewport);
  const auto p3 = ToTexelPos(ndc_p3, viewport);

  const auto bounding_box = GetBoundingBox(p1, p2, p3);
  for (auto y = 0; y < bounding_box.size.height; y++) {
    for (auto x = 0; x < bounding_box.size.width; x++) {
      const auto p =
          glm::vec2{x + bounding_box.origin.x, y + bounding_box.origin.y};
      const auto bary = GetBaryCentricCoordinates(p, p1, p2, p3);
      if (bary.x < 0.0 || bary.y < 0.0 || bary.z < 0.0) {
        continue;
      }
      auto color = pipeline_->shader->ProcessFragment(bary, 0u);
      if (!color.has_value()) {
        continue;
      }
      const auto bary_pos =
          BarycentricInterpolation(ndc_p1, ndc_p2, ndc_p3, bary);
      Texel texel;
      texel.pos = p;
      texel.depth = bary_pos.z;
      texel.color = color.value();
      UpdateTexel(texel);
    }
  }
}

void Rasterizer::SetPipeline(std::shared_ptr<Pipeline> pipeline) {
  pipeline_ = pipeline;
}

}  // namespace sft
