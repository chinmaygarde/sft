#include "rasterizer.h"

#include "invocation.h"
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

void Rasterizer::UpdateTexel(const Pipeline& pipeline, Texel texel) {
  if (IsOOB(texel.pos, size_)) {
    return;
  }

  const auto offset = size_.x * texel.pos.y + texel.pos.x;

  auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;
  auto depth_ptr = reinterpret_cast<uint32_t*>(depth_buffer_) + offset;

  auto new_depth = Color::Gray(texel.depth);

  if (pipeline.depth_test_enabled) {
    const auto old_depth = Color(depth_ptr[0]);
    if (new_depth.GetRed() > old_depth.GetRed()) {
      return;
    }
  }

  *color_ptr = pipeline.Blend(texel.color, *color_ptr);
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
  glm::vec2 ab = b - a, ac = c - a, ap = p - a;
  float one_over_den = 1.0f / (ab.x * ac.y - ab.y * ac.x);
  float s = (ac.y * ap.x - ac.x * ap.y) * one_over_den;
  float t = (ab.x * ap.y - ab.y * ap.x) * one_over_den;
  return {1.0f - s - t, s, t};
}

constexpr bool ShouldCullFace(CullFace face,
                              Winding winding,
                              glm::vec3 a,
                              glm::vec3 b,
                              glm::vec3 c) {
  auto dir = glm::cross(b - a, c - a).z;
  const bool is_front = face == CullFace::kFront;
  const bool is_cw = winding == Winding::kClockwise;
  if (!is_front) {
    dir = -dir;
  }
  if (!is_cw) {
    dir = -dir;
  }
  return dir < 0;
}

void Rasterizer::DrawTriangle(const TriangleData& data) {
  auto viewport = data.pipeline.viewport.value_or(size_);

  //----------------------------------------------------------------------------
  // Invoke vertex shaders.
  //----------------------------------------------------------------------------
  VertexInvocation vertex_invocation(*this, data, data.vertex_id);
  const auto ndc_p1 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto ndc_p2 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto ndc_p3 = data.pipeline.shader->ProcessVertex(vertex_invocation);

  //----------------------------------------------------------------------------
  // Cull faces.
  //----------------------------------------------------------------------------
  if (data.pipeline.cull_face.has_value()) {
    if (ShouldCullFace(data.pipeline.cull_face.value(),  //
                       data.pipeline.winding,            //
                       ndc_p1,                           //
                       ndc_p2,                           //
                       ndc_p3                            //
                       )) {
      return;
    }
  }

  //----------------------------------------------------------------------------
  // Convert NDC points returned by the shader into screen-space.
  //----------------------------------------------------------------------------
  const auto p1 = ToTexelPos(ndc_p1, viewport);
  const auto p2 = ToTexelPos(ndc_p2, viewport);
  const auto p3 = ToTexelPos(ndc_p3, viewport);

  //----------------------------------------------------------------------------
  // Find bounding box and apply scissor.
  //----------------------------------------------------------------------------
  const auto bounding_box =
      GetBoundingBox(p1, p2, p3)
          .Intersection(data.pipeline.scissor.value_or(Rect{size_}));

  if (!bounding_box.has_value()) {
    return;
  }

  //----------------------------------------------------------------------------
  // Shade fragments.
  //----------------------------------------------------------------------------
  for (auto y = 0; y < bounding_box->size.height; y++) {
    for (auto x = 0; x < bounding_box->size.width; x++) {
      const auto p = glm::vec2{x + 1.0f + bounding_box->origin.x,
                               y + 1.0f + bounding_box->origin.y};
      const auto bary = GetBaryCentricCoordinates(p, p1, p2, p3);
      if (bary.x < 0 || bary.y < 0 || bary.z < 0) {
        continue;
      }
      const auto color =
          Color{data.pipeline.shader->ProcessFragment({bary, *this, data})};
      const auto bary_pos =
          BarycentricInterpolation(ndc_p1, ndc_p2, ndc_p3, bary);
      Texel texel;
      texel.pos = p;
      texel.depth = bary_pos.z;
      texel.color = color;
      UpdateTexel(data.pipeline, texel);
    }
  }
}

void Rasterizer::Draw(const Pipeline& pipeline,
                      const Buffer& vertex_buffer,
                      const Buffer& uniform_buffer,
                      size_t count) {
  const auto& vtx_desc = pipeline.vertex_descriptor;
  const uint8_t* vtx_ptr = vertex_buffer.GetData() + vtx_desc.offset;
  const auto varyings_size = pipeline.shader->GetVaryingsSize();
  auto* varyings = reinterpret_cast<uint8_t*>(::alloca(varyings_size * 3u));
  TriangleData data(pipeline, vertex_buffer, uniform_buffer, varyings_size,
                    varyings);
  size_t vertex_id = 0;
  glm::vec3 p1, p2, p3;
  for (size_t i = 0; i < count; i += 3) {
    data.vertex_id = vertex_id;
    vertex_id += 3;
    memcpy(&data.p1, vtx_ptr, sizeof(p1));
    vtx_ptr += vtx_desc.stride;
    memcpy(&data.p2, vtx_ptr, sizeof(p2));
    vtx_ptr += vtx_desc.stride;
    memcpy(&data.p3, vtx_ptr, sizeof(p3));
    DrawTriangle(data);
  }
}

}  // namespace sft
