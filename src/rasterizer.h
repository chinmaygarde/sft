#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "buffer_view.h"
#include "geom.h"
#include "pipeline.h"
#include "rasterizer_metrics.h"
#include "render_pass.h"
#include "stage_resources.h"
#include "texture.h"
#include "tiler.h"

namespace sft {

class Image;

class Rasterizer {
 public:
  Rasterizer(glm::ivec2 size, SampleCount sample_count);

  ~Rasterizer();

  RenderPassAttachments& GetRenderPass();

  glm::ivec2 GetSize() const;

  void Clear(Color color);

  void Finish() { tiler_.Dispatch(*this); }

  void Draw(std::shared_ptr<Pipeline> pipeline,
            const BufferView& vertex_buffer,
            const BufferView& uniform_buffer,
            size_t count,
            uint32_t stencil_refernece = 0) {
    return Draw(std::move(pipeline), vertex_buffer, {}, uniform_buffer, count,
                stencil_refernece);
  }

  void Draw(std::shared_ptr<Pipeline> pipeline,
            const BufferView& vertex_buffer,
            const BufferView& index_buffer,
            const BufferView& uniform_buffer,
            size_t count,
            uint32_t stencil_reference = 0) {
    metrics_.draw_count++;
    auto resources = std::make_shared<DispatchResources>(
        pipeline->shader->GetVaryingsSize());
    resources->vertex = std::move(vertex_buffer);
    resources->index = std::move(index_buffer);
    resources->uniform = std::move(uniform_buffer);
    VertexResources data(pipeline,              //
                         std::move(resources),  //
                         stencil_reference      //
    );
    const auto vtx_offset = pipeline->vertex_descriptor.offset;
    for (size_t i = 0; i < count; i += 3) {
      data.base_vertex_id = i;
      data.vtx[0] = data.GetVertexData<glm::vec3>(i + 0, vtx_offset);
      data.vtx[1] = data.GetVertexData<glm::vec3>(i + 1, vtx_offset);
      data.vtx[2] = data.GetVertexData<glm::vec3>(i + 2, vtx_offset);
      DrawTriangle(data);
    }
  }

  template <class T>
  void StoreVarying(const DispatchResources& resources,
                    const T& val,
                    size_t triangle_index,
                    size_t struct_offset) const {
    auto varyings_offset =
        struct_offset + resources.GetVaryingsStride() * (triangle_index % 3);
    auto ptr =
        const_cast<uint8_t*>(resources.varyings.data()) + varyings_offset;
    memcpy(ptr, &val, sizeof(T));
  }

  template <class T>
  T LoadVarying(const DispatchResources& resources,
                const glm::vec3& barycentric_coordinates,
                size_t struct_offset) const {
    const auto stride = resources.GetVaryingsStride();
    auto ptr = resources.varyings.data() + struct_offset;
    T p1, p2, p3;
    memcpy(&p1, ptr, sizeof(p1));
    ptr += stride;
    memcpy(&p2, ptr, sizeof(p2));
    ptr += stride;
    memcpy(&p3, ptr, sizeof(p3));
    return BarycentricInterpolation(p1, p2, p3, barycentric_coordinates);
  }

  template <class T>
  T LoadVertexData(const VertexResources& data,
                   size_t index,
                   size_t offset) const {
    return data.GetVertexData<T>(index, offset);
  }

  template <class T>
  T LoadUniform(const DispatchResources& resources, size_t offset) const {
    T result = {};
    memcpy(&result, resources.uniform.GetData() + offset, sizeof(T));
    return result;
  }

  void ResetMetrics();

  const RasterizerMetrics& GetMetrics() const;

  std::shared_ptr<Image> CaptureDebugDepthTexture() const;

  std::shared_ptr<Image> CaptureDebugStencilTexture() const;

  [[nodiscard]] bool Resize(glm::ivec2 size);

  [[nodiscard]] bool ResizeSamples(SampleCount count);

  void ShadeFragments(const FragmentResources& tiler_data, const Rect& tile);

 private:
  RenderPassAttachments pass_;
  glm::ivec2 size_;
  RasterizerMetrics metrics_;
  Tiler tiler_;

  bool FragmentPassesDepthTest(const Pipeline& pipeline,
                               glm::ivec2 pos,
                               ScalarF depth,
                               size_t sample) const;

  bool UpdateAndCheckFragmentPassesStencilTest(const Pipeline& pipeline,
                                               glm::ivec2 pos,
                                               bool depth_test_passes,
                                               uint32_t reference_value,
                                               size_t sample);

  void UpdateColor(const ColorAttachmentDescriptor& color_desc,
                   const glm::ivec2& pos,
                   const Color& color,
                   size_t sample);

  void UpdateDepth(const DepthAttachmentDescriptor& depth_desc,
                   const glm::ivec2& pos,
                   ScalarF depth,
                   size_t sample);

  void DrawTriangle(const VertexResources& data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
