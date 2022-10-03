#include "rasterizer.h"
#include <cfloat>

#include "invocation.h"
#include "macros.h"
#include "mapping.h"
#include "texture.h"

namespace sft {

Rasterizer::Rasterizer(glm::ivec2 size)
    : color0_(size), depth0_(size), stencil0_(size), size_(size) {}

Rasterizer::~Rasterizer() = default;

const void* Rasterizer::GetPixels() const {
  return color0_.Get();
}

glm::ivec2 Rasterizer::GetSize() const {
  return size_;
}

size_t Rasterizer::GetBytesPerPixel() const {
  return color0_.GetBytesPerPixel();
}

constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
  return pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y;
}

bool Rasterizer::FragmentPassesDepthTest(const Pipeline& pipeline,
                                         glm::ivec2 pos,
                                         ScalarF new_value) const {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.depth_desc.depth_test_enabled) {
    return true;
  }

  const auto current_value = *depth0_.Get(pos);

  return CompareFunctionPasses(pipeline.depth_desc.depth_compare,  //
                               new_value,                          //
                               current_value                       //
  );
}

bool Rasterizer::FragmentPassesStencilTest(const Pipeline& pipeline,
                                           glm::ivec2 pos,
                                           uint32_t reference_value) const {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.stencil_desc.stencil_test_enabled) {
    return true;
  }

  const auto current_value = *stencil0_.Get(pos);

  return CompareFunctionPasses(pipeline.stencil_desc.stencil_compare,  //
                               reference_value,                        //
                               current_value                           //
  );
}

void Rasterizer::UpdateTexel(const Pipeline& pipeline, Texel texel) {
  if (IsOOB(texel.pos, size_)) {
    return;
  }

  //----------------------------------------------------------------------------
  // Write to the color attachment.
  //----------------------------------------------------------------------------
  auto dst = *color0_.Get(texel.pos);
  auto src = texel.color;
  auto color = pipeline.color_desc.blend.Blend(src, dst);
  color0_.Set(color, texel.pos);

  //----------------------------------------------------------------------------
  // Write to the depth attachment.
  //----------------------------------------------------------------------------
  if (pipeline.depth_desc.depth_test_enabled) {
    if (pipeline.depth_desc.depth_write_enabled) {
      depth0_.Set(texel.depth, texel.pos);
    }
  }
}

void Rasterizer::Clear(Color color) {
  color0_.Clear(color);
  depth0_.Clear(1);
  stencil0_.Clear(0);
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
  return glm::clamp((z_depth + 1.0) / 2.0, 0.0, 1.0);
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
      // Perform the depth test.
      //------------------------------------------------------------------------
      const auto bary_pos =
          BarycentricInterpolation(ndc_p1, ndc_p2, ndc_p3, bary);
      const auto depth = NormalizeDepth(bary_pos.z);
      const auto depth_test_passes =
          FragmentPassesDepthTest(data.pipeline, pos, depth);

      //------------------------------------------------------------------------
      // Perform the stencil test.
      //------------------------------------------------------------------------
      const auto stencil_test_passes =
          FragmentPassesStencilTest(data.pipeline, pos, data.stencil_reference);

      //------------------------------------------------------------------------
      // Determine the new stencil value.
      //------------------------------------------------------------------------
      const auto stencil_op =
          data.pipeline.stencil_desc.SelectOperation(depth_test_passes,   //
                                                     stencil_test_passes  //
          );
      const auto stencil_val =
          StencilOperationPerform(
              stencil_op,  // selected stencil operation
              *stencil0_.Get(pos) & data.pipeline.stencil_desc
                                        .read_mask,  // current stencil value
              data.stencil_reference                 // stencil reference value
              ) &
          data.pipeline.stencil_desc.write_mask;

      //------------------------------------------------------------------------
      // Update the stencil value.
      //------------------------------------------------------------------------
      stencil0_.Set(stencil_val, pos);

      //------------------------------------------------------------------------
      // If either the depth stencil tests have failed, short circuit fragment
      // processing.
      //------------------------------------------------------------------------
      if (!stencil_test_passes || !depth_test_passes) {
        metrics_.early_fragment_test++;
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
  auto depth_bytes = depth0_.Get();

  ScalarF min = 1;
  ScalarF max = 0;
  for (auto i = 0; i < texel_count; i++) {
    ScalarF level = depth_bytes[i];
    min = glm::min(min, level);
    max = glm::max(max, level);
  }
  for (auto i = 0; i < texel_count; i++) {
    const auto level = glm::lerp(min, max, depth_bytes[i]);
    debug_tex_buf[i] = Color::FromComponentsF(level, level, level, 1.0);
  }
  return std::make_shared<Texture>(std::move(debug_tex_mapping), size_);
}

}  // namespace sft
