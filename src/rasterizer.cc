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

bool Rasterizer::UpdateAndCheckFragmentPassesStencilTest(
    const Pipeline& pipeline,
    glm::ivec2 pos,
    bool depth_test_passes,
    uint32_t reference_value) {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.stencil_desc.stencil_test_enabled) {
    return true;
  }

  const auto read_mask = pipeline.stencil_desc.read_mask;
  const auto write_mask = pipeline.stencil_desc.write_mask;

  const auto current_value = *stencil0_.Get(pos);

  const auto stencil_test_passes =
      CompareFunctionPasses(pipeline.stencil_desc.stencil_compare,  //
                            read_mask & current_value,              //
                            read_mask & reference_value             //
      );

  //------------------------------------------------------------------------
  // Determine the new stencil value.
  //------------------------------------------------------------------------
  const auto stencil_op =
      pipeline.stencil_desc.SelectOperation(depth_test_passes,   //
                                            stencil_test_passes  //
      );

  const auto new_stencil_value =
      StencilOperationPerform(
          stencil_op,                  // selected stencil operation
          read_mask & current_value,   // current stencil value
          read_mask & reference_value  // stencil reference value
          ) &
      write_mask;

  //------------------------------------------------------------------------
  // Update the stencil value.
  //------------------------------------------------------------------------
  stencil0_.Set(new_stencil_value, pos);

  return stencil_test_passes;
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
  metrics_.area = color0_.GetSize();
}

constexpr glm::vec3 ToNDC(const glm::vec4& clip) {
  auto ndc = glm::vec3{clip};
  ndc /= clip.w;
  return ndc;
}

constexpr glm::vec2 ToTexelPos(const glm::vec3& ndc,
                               const glm::vec2& viewport) {
  return {
      (viewport.x / 2.0f) * (ndc.x + 1.0f),  //
      (viewport.y / 2.0f) * (ndc.y + 1.0f),  //
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
  // Invoke vertex shaders. The clip-space coordinates returned are specified by
  // homogenous 4D vectors.
  //----------------------------------------------------------------------------
  VertexInvocation vertex_invocation(*this, data, data.base_vertex_id);
  const auto clip_p1 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto clip_p2 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vertex_id++;
  const auto clip_p3 = data.pipeline.shader->ProcessVertex(vertex_invocation);
  metrics_.vertex_invocations += 3;

  //----------------------------------------------------------------------------
  // Convert clip space coordinates into NDC coordinates (divide by w).
  //----------------------------------------------------------------------------
  const auto ndc_p1 = ToNDC(clip_p1);
  const auto ndc_p2 = ToNDC(clip_p2);
  const auto ndc_p3 = ToNDC(clip_p3);

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
  const auto frag_p1 = ToTexelPos(ndc_p1, viewport);
  const auto frag_p2 = ToTexelPos(ndc_p2, viewport);
  const auto frag_p3 = ToTexelPos(ndc_p3, viewport);

  //----------------------------------------------------------------------------
  // Find bounding box and apply scissor.
  //----------------------------------------------------------------------------
  const auto bounding_box = GetBoundingBox(frag_p1, frag_p2, frag_p3);

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

  metrics_.primitives_processed++;

  //----------------------------------------------------------------------------
  // Shade fragments.
  //----------------------------------------------------------------------------
  for (auto y = box.origin.y; y <= box.origin.y + box.size.height; y++) {
    for (auto x = box.origin.x; x <= box.origin.x + box.size.width; x++) {
      const auto frag = glm::vec2{x + 0.5f, y + 0.5f};
      const auto bary =
          GetBaryCentricCoordinates(frag, frag_p1, frag_p2, frag_p3);
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
          BarycentricInterpolation(clip_p1, clip_p2, clip_p3, bary);
      const auto depth = NormalizeDepth(bary_pos.z);
      const auto depth_test_passes =
          FragmentPassesDepthTest(data.pipeline, frag, depth);

      //------------------------------------------------------------------------
      // Perform the stencil test.
      //------------------------------------------------------------------------
      const auto stencil_test_passes =
          UpdateAndCheckFragmentPassesStencilTest(data.pipeline,          //
                                                  frag,                   //
                                                  depth_test_passes,      //
                                                  data.stencil_reference  //
          );

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
      texel.pos = frag;
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

template <class T>
constexpr Color CreateDebugColor(T val, T min, T max) {
  const auto full_range = max - min;
  const auto range = val - min;
  if (full_range == 0) {
    return kColorRed;
  }
  const auto component = static_cast<ScalarF>(range) / full_range;
  return Color::FromComponentsF(component, component, component, 1.0);
}

std::shared_ptr<Texture> Rasterizer::CaptureDebugDepthTexture() const {
  const auto min_max = depth0_.GetMinMaxValue();
  return depth0_.CreateTexture([&min_max](const ScalarF& val) -> Color {
    return CreateDebugColor(val, min_max.first, min_max.second);
  });
}

std::shared_ptr<Texture> Rasterizer::CaptureDebugStencilTexture() const {
  const auto min_max = stencil0_.GetMinMaxValue();
  return stencil0_.CreateTexture([&min_max](const uint32_t& val) -> Color {
    return CreateDebugColor(val, min_max.first, min_max.second);
  });
}

bool Rasterizer::Resize(glm::ivec2 size) {
  if (size_ == size) {
    return true;
  }

  if (!color0_.Resize(size) || !depth0_.Resize(size) ||
      !stencil0_.Resize(size)) {
    return false;
  }

  size_ = size;
  return true;
}

}  // namespace sft
