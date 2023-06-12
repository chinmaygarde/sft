/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "rasterizer.h"
#include <cfloat>

#include "image.h"
#include "invocation.h"
#include "macros.h"
#include "mapping.h"

namespace sft {

Rasterizer::Rasterizer(glm::ivec2 size, SampleCount sample_count)
    : pass_(size, sample_count) {
  size_ = pass_.GetSize();
}

Rasterizer::~Rasterizer() = default;

glm::ivec2 Rasterizer::GetSize() const {
  return pass_.GetSize();
}

constexpr bool IsOOB(glm::ivec2 pos, glm::ivec2 size) {
  return pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y;
}

bool Rasterizer::FragmentPassesDepthTest(const Pipeline& pipeline,
                                         glm::ivec2 pos,
                                         ScalarF new_value,
                                         size_t sample) const {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.depth_desc.depth_test_enabled) {
    return true;
  }

  const auto current_value = *pass_.depth.texture->Get(pos, sample);

  return CompareFunctionPasses(pipeline.depth_desc.depth_compare,  //
                               new_value,                          //
                               current_value                       //
  );
}

bool Rasterizer::UpdateAndCheckFragmentPassesStencilTest(
    const Pipeline& pipeline,
    glm::ivec2 pos,
    bool depth_test_passes,
    uint32_t reference_value,
    size_t sample) {
  if (IsOOB(pos, size_)) {
    return false;
  }

  if (!pipeline.stencil_desc.stencil_test_enabled) {
    return true;
  }

  const auto read_mask = pipeline.stencil_desc.read_mask;
  const auto write_mask = pipeline.stencil_desc.write_mask;

  const auto current_value = *pass_.stencil.texture->Get(pos, sample);

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
  pass_.stencil.texture->Set(new_stencil_value, pos, sample);

  return stencil_test_passes;
}

void Rasterizer::UpdateColor(const ColorAttachmentDescriptor& color_desc,
                             const glm::ivec2& pos,
                             const Color& src,
                             size_t sample) {
  if (IsOOB(pos, size_)) {
    return;
  }
  if (color_desc.blend.enabled) {
    auto dst = *pass_.color.texture->Get(pos, sample);
    auto color = color_desc.blend.Blend(src, dst);
    pass_.color.texture->Set(color, pos, sample);
  } else {
    pass_.color.texture->Set(src, pos, sample);
  }
}

void Rasterizer::UpdateDepth(const DepthAttachmentDescriptor& depth_desc,
                             const glm::ivec2& pos,
                             ScalarF depth,
                             size_t sample) {
  if (IsOOB(pos, size_)) {
    return;
  }
  if (!depth_desc.depth_test_enabled) {
    return;
  }
  if (!depth_desc.depth_write_enabled) {
    return;
  }
  pass_.depth.texture->Set(depth, pos, sample);
}

void Rasterizer::Clear(Color color) {
  pass_.color.clear_color = color;
  pass_.Load();
  metrics_.area = pass_.GetSize();
  tiler_.Reset();
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
/// @brief      Return if a point `p` is on the left, right or on the line from
///             `v0` to `v1`.
///
/// @param[in]  v0    The point v0.
/// @param[in]  v1    The point v1.
/// @param[in]  p     The point p.
///
/// @return     Less than 0 if on the left, greater than zero if one the right
///             and zero if on the line.
///
constexpr ScalarF EdgeFunction(const glm::vec2& v0,
                               const glm::vec2& v1,
                               const glm::vec2& p) {
  return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

static bool IsTopLeftEdge(const glm::vec2& edge) {
  // Top edges are flat (y == 0) and go right (x == 0).
  const auto is_top =
      glm::epsilonEqual(edge.y, 0.0f, kEpsilon) && edge.x > 0.0f;

  // Left edges are ones that go up.
  const auto is_left = edge.y > 0.0f;

  return is_top || is_left;
}

static bool PointInside(const glm::vec2& a,
                        const glm::vec2& b,
                        const glm::vec2& c,
                        const glm::vec2& p) {
  const auto edge_ab = EdgeFunction(a, b, p);
  const auto edge_bc = EdgeFunction(b, c, p);
  const auto edge_ca = EdgeFunction(c, a, p);

  // The point is clearly not in the triangle or an edge.
  if (edge_ab < -kEpsilon || edge_bc < -kEpsilon || edge_ca < -kEpsilon) {
    return false;
  }

  // Check if the triangle is on the edge. If it is, we need to apply the
  // Top-Left rule.
  // https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules
  const auto on_ab = glm::epsilonEqual(edge_ab, 0.0f, kEpsilon);
  const auto on_bc = glm::epsilonEqual(edge_bc, 0.0f, kEpsilon);
  const auto on_ca = glm::epsilonEqual(edge_ca, 0.0f, kEpsilon);

  const auto ab = b - a;
  const auto bc = c - b;
  const auto ca = a - c;

  if (on_ab && !IsTopLeftEdge(ab)) {
    return false;
  }

  if (on_bc && !IsTopLeftEdge(bc)) {
    return false;
  }

  if (on_ca && !IsTopLeftEdge(ca)) {
    return false;
  }

  return true;
}

void Rasterizer::ShadeFragments(const FragmentResources& tiler_data,
                                const Rect& tile) {
  auto box = tiler_data.box.Intersection(tile);
  if (!box.has_value()) {
    return;
  }
  const auto sample_count = pass_.color.texture->GetSampleCount();
  const auto& pipeline = tiler_data.pipeline;
  auto viewport = pipeline->viewport.value_or(size_);
  const auto frag_p1 = ToTexelPos(tiler_data.ndc[0], viewport);
  const auto frag_p2 = ToTexelPos(tiler_data.ndc[1], viewport);
  const auto frag_p3 = ToTexelPos(tiler_data.ndc[2], viewport);
  //----------------------------------------------------------------------------
  // Shade fragments.
  //----------------------------------------------------------------------------
  for (auto y = box->origin.y; y <= box->origin.y + box->size.height; y++) {
    for (auto x = box->origin.x; x <= box->origin.x + box->size.width; x++) {
      const auto pixel = glm::vec2{x, y};
      uint32_t samples_found = 0;

      for (size_t sample = 0; sample < GetSampleCount(sample_count); sample++) {
        const auto frag = pixel + GetSampleLocation(sample_count, sample);

        if (!PointInside(frag_p1, frag_p2, frag_p3, frag)) {
          continue;
        }

        //----------------------------------------------------------------------
        // Perform the depth test.
        //----------------------------------------------------------------------
        const auto bary =
            GetBaryCentricCoordinates(frag, frag_p1, frag_p2, frag_p3);
        const auto depth = BarycentricInterpolation(tiler_data.ndc[0],  //
                                                    tiler_data.ndc[1],  //
                                                    tiler_data.ndc[2],  //
                                                    bary                //
                                                    )
                               .z;
        const auto depth_test_passes =
            FragmentPassesDepthTest(*pipeline, frag, depth, sample);

        //----------------------------------------------------------------------
        // Perform the stencil test.
        //----------------------------------------------------------------------
        const auto stencil_test_passes =
            UpdateAndCheckFragmentPassesStencilTest(
                *pipeline,                     //
                frag,                          //
                depth_test_passes,             //
                tiler_data.stencil_reference,  //
                sample                         //
            );

        //----------------------------------------------------------------------
        // If either the depth stencil tests have failed, short circuit fragment
        // processing.
        //----------------------------------------------------------------------
        if (!stencil_test_passes || !depth_test_passes) {
          metrics_.early_fragment_test++;
          continue;
        }

        //----------------------------------------------------------------------
        // Update the depth values.
        //----------------------------------------------------------------------
        UpdateDepth(pipeline->depth_desc, frag, depth, sample);

        //----------------------------------------------------------------------
        // This sample location needs a color value.
        //----------------------------------------------------------------------
        samples_found |= (1 << sample);
      }

      if (samples_found == 0) {
        continue;
      }

      //------------------------------------------------------------------------
      // Shade the fragment. But just once for all samples.
      //------------------------------------------------------------------------
      const auto frag = pixel + kSampleMidpoint;
      const auto bary =
          GetBaryCentricCoordinates(frag, frag_p1, frag_p2, frag_p3);
      const auto color =
          Color{pipeline->shader->ProcessFragment({bary, tiler_data})};
      metrics_.fragment_invocations++;

      //------------------------------------------------------------------------
      // Blend in the color for found samples.
      //------------------------------------------------------------------------
      for (size_t sample = 0; sample < GetSampleCount(sample_count); sample++) {
        if (samples_found & (1 << sample)) {
          const auto frag = pixel + GetSampleLocation(sample_count, sample);
          UpdateColor(pipeline->color_desc, frag, color, sample);
        }
      }
    }
  }
}

void Rasterizer::DrawTriangle(const VertexResources& data) {
  metrics_.primitive_count++;

  auto tiler_data = FragmentResources{data.pipeline->shader->GetVaryingsSize()};
  tiler_data.stencil_reference = data.stencil_reference;
  tiler_data.pipeline = data.pipeline;
  tiler_data.resources = data.resources;

  //----------------------------------------------------------------------------
  // Invoke vertex shaders. The clip-space coordinates returned are specified by
  // homogenous 4D vectors.
  //----------------------------------------------------------------------------
  VertexInvocation vertex_invocation(data, tiler_data, data.base_vertex_id);
  const auto clip_p1 = data.pipeline->shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vtx_index++;
  const auto clip_p2 = data.pipeline->shader->ProcessVertex(vertex_invocation);
  vertex_invocation.vtx_index++;
  const auto clip_p3 = data.pipeline->shader->ProcessVertex(vertex_invocation);
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
  if (data.pipeline->cull_face.has_value()) {
    if (ShouldCullFace(data.pipeline->cull_face.value(),  //
                       data.pipeline->winding,            //
                       ndc_p1,                            //
                       ndc_p2,                            //
                       ndc_p3                             //
                       )) {
      metrics_.face_culling++;
      return;
    }
  }

  //----------------------------------------------------------------------------
  // Convert NDC points returned by the shader into screen-space.
  //----------------------------------------------------------------------------
  auto viewport = data.pipeline->viewport.value_or(size_);
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
      bounding_box.Intersection(data.pipeline->scissor.value_or(Rect{size_}));

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

  tiler_data.box = box;
  tiler_data.ndc[0] = ndc_p1;
  tiler_data.ndc[1] = ndc_p2;
  tiler_data.ndc[2] = ndc_p3;

  tiler_.AddData(std::move(tiler_data));
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

bool Rasterizer::Resize(glm::ivec2 size) {
  if (size_ == size) {
    return true;
  }
  if (!pass_.Resize(size)) {
    return false;
  }
  size_ = size;
  return true;
}

RenderPassAttachments& Rasterizer::GetRenderPass() {
  return pass_;
}

}  // namespace sft
