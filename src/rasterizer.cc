#include "rasterizer.h"
#include <cfloat>

#include "invocation.h"
#include "macros.h"
#include "mapping.h"
#include "texture.h"

namespace sft {

Rasterizer::Rasterizer(glm::ivec2 size)
    : color_buffer_(std::calloc(size.x * size.y, sizeof(Color))),
      depth_buffer_(std::calloc(size.x * size.y, sizeof(ScalarF))),
      size_(size) {}

Rasterizer::~Rasterizer() {
  std::free(depth_buffer_);
  std::free(color_buffer_);
}

void* Rasterizer::GetPixels() const {
  return color_buffer_;
}

glm::ivec2 Rasterizer::GetSize() const {
  return size_;
}

size_t Rasterizer::GetBytesPerPixel() const {
  return sizeof(uint32_t);
}

constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
  return pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y;
}

bool Rasterizer::FragmentPassesDepthTest(const Pipeline& pipeline,
                                         glm::ivec2 pos,
                                         ScalarF depth) const {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.depth_test_enabled) {
    return true;
  }

  const auto offset = size_.x * pos.y + pos.x;
  auto depth_ptr = reinterpret_cast<ScalarF*>(depth_buffer_) + offset;
  const auto old_depth = depth_ptr[0];
  if (depth < old_depth) {
    return false;
  }
  return true;
}

void Rasterizer::UpdateTexel(const Pipeline& pipeline, Texel texel) {
  if (IsOOB(texel.pos, size_)) {
    return;
  }

  const auto offset = size_.x * texel.pos.y + texel.pos.x;

  //----------------------------------------------------------------------------
  // Write to the color attachment.
  //----------------------------------------------------------------------------
  auto color_ptr = reinterpret_cast<Color*>(color_buffer_) + offset;
  *color_ptr = pipeline.blend.Blend(texel.color, *color_ptr);

  //----------------------------------------------------------------------------
  // Write to the depth attachment.
  //----------------------------------------------------------------------------
  if (pipeline.depth_test_enabled) {
    auto depth_ptr = reinterpret_cast<ScalarF*>(depth_buffer_) + offset;
    *depth_ptr = texel.depth;
  }
}

void Rasterizer::Clear(Color color) {
  memset_pattern4(color_buffer_, &color, size_.x * size_.y * sizeof(Color));
  bzero(depth_buffer_, size_.x * size_.y * sizeof(ScalarF));
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

//------------------------------------------------------------------------------
/// @brief      Normalizes the Z value in the NDC cube to unit-scale.
///
/// @param[in]  z_depth  The z depth in the NDC cube.
///
/// @return     The depth value in unit-scale.
///
constexpr ScalarF NormalizeDepth(ScalarF z_depth) {
  return 1.0 - glm::clamp((z_depth + 1.0) / 2.0, 0.0, 1.0);
}

void Rasterizer::DrawTriangle(const TriangleData& data) {
  metrics_.primitive_count++;

  auto viewport = data.pipeline.viewport.value_or(size_);

  //----------------------------------------------------------------------------
  // Invoke vertex shaders.
  //----------------------------------------------------------------------------
  VertexInvocation vertex_invocation(*this, data, data.base_vertex_id);
  const auto ndc_p1 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto ndc_p2 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto ndc_p3 = data.pipeline.shader->ProcessVertex(vertex_invocation);

  metrics_.vertex_invocations += 3;

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
      metrics_.face_culling++;
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
  const auto bounding_box = GetBoundingBox(p1, p2, p3);

  if (bounding_box.size.IsEmpty()) {
    metrics_.empty_primitive++;
    return;
  }

  auto scissor_box =
      bounding_box.Intersection(data.pipeline.scissor.value_or(Rect{size_}));

  if (!scissor_box.has_value()) {
    metrics_.scissor_culling++;
    return;
  }

  const auto& box = scissor_box.value();

  //----------------------------------------------------------------------------
  // Apply sample point culling.
  // From https://developer.arm.com/documentation/102540/0100/Primitive-culling
  //----------------------------------------------------------------------------
  if (box.size.width < 2 && box.size.height < 2) {
    metrics_.sample_point_culling++;
    return;
  }

  //----------------------------------------------------------------------------
  // Shade fragments.
  //----------------------------------------------------------------------------
  for (auto y = 0; y < box.size.height; y++) {
    for (auto x = 0; x < box.size.width; x++) {
      const auto pos =
          glm::vec2{x + 1.0f + box.origin.x, y + 1.0f + box.origin.y};
      const auto bary = GetBaryCentricCoordinates(pos, p1, p2, p3);
      //------------------------------------------------------------------------
      // Check if the fragment falls within the triangle.
      //------------------------------------------------------------------------
      if (bary.x < 0 || bary.y < 0 || bary.z < 0) {
        continue;
      }

      //------------------------------------------------------------------------
      // If the depth test fails, short circuit processing the shader for the
      // fragment.
      //------------------------------------------------------------------------
      const auto bary_pos =
          BarycentricInterpolation(ndc_p1, ndc_p2, ndc_p3, bary);
      const auto depth = NormalizeDepth(bary_pos.z);
      if (!FragmentPassesDepthTest(data.pipeline, pos, depth)) {
        continue;
      }

      //------------------------------------------------------------------------
      // Shade the fragment.
      //------------------------------------------------------------------------
      const auto color =
          Color{data.pipeline.shader->ProcessFragment({bary, *this, data})};
      metrics_.fragment_invocations++;

      //------------------------------------------------------------------------
      // Update the texel.
      //------------------------------------------------------------------------
      Texel texel;
      texel.pos = pos;
      texel.depth = depth;
      texel.color = color;
      UpdateTexel(data.pipeline, texel);
    }
  }
}

void Rasterizer::ResetMetrics() {
  metrics_.Reset();
}

const RasterizerMetrics& Rasterizer::GetMetrics() const {
  return metrics_;
}

std::shared_ptr<Texture> Rasterizer::CaptureDebugDepthTexture() const {
  const auto texel_count = size_.x * size_.y;
  const auto debug_tex_bytes = texel_count * sizeof(Color);
  auto debug_tex_buf = reinterpret_cast<Color*>(std::malloc(debug_tex_bytes));
  if (!debug_tex_buf) {
    return nullptr;
  }
  auto debug_tex_mapping = std::make_shared<Mapping>(
      reinterpret_cast<const uint8_t*>(debug_tex_buf),  //
      debug_tex_bytes,                                  //
      [debug_tex_buf]() { std::free(debug_tex_buf); });
  auto depth_bytes = reinterpret_cast<ScalarF*>(depth_buffer_);
  for (auto i = 0; i < texel_count; i++) {
    const auto level = glm::clamp(depth_bytes[i], 0.0f, 1.0f);
    debug_tex_buf[i] = Color::FromComponentsF(level, level, level, 1.0);
  }
  return std::make_shared<Texture>(std::move(debug_tex_mapping), size_);
}

}  // namespace sft
