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
#include "texture.h"
#include "tiler.h"
#include "triangle_data.h"

namespace sft {

class Image;

class Rasterizer {
 public:
  Rasterizer(glm::ivec2 size, SampleCount sample_count);

  ~Rasterizer();

  RenderPassAttachments& GetRenderPass();

  glm::ivec2 GetSize() const;

  void Clear(Color color);

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
    const auto varyings_size = pipeline->shader->GetVaryingsSize();
    VertexData data(pipeline,  //
                    Bindings{
                        vertex_buffer,   //
                        index_buffer,    //
                        uniform_buffer,  //
                    },                   //
                    varyings_size,       //
                    stencil_reference    //
    );
    const auto vtx_offset = pipeline->vertex_descriptor.offset;
    for (size_t i = 0; i < count; i += 3) {
      data.base_vertex_id = i;
      data.p1 = data.GetVertexData<decltype(data.p1)>(i + 0, vtx_offset);
      data.p2 = data.GetVertexData<decltype(data.p2)>(i + 1, vtx_offset);
      data.p3 = data.GetVertexData<decltype(data.p3)>(i + 2, vtx_offset);
      DrawTriangle(data);
    }
  }

  template <class T>
  void StoreVarying(const VertexData& data,
                    const T& val,
                    size_t index,
                    size_t offset) const {
    auto varyings_offset = offset + data.varyings_stride * (index % 3);
    auto ptr = const_cast<uint8_t*>(data.varyings.data()) + varyings_offset;
    memcpy(ptr, &val, sizeof(T));
  }

  template <class T>
  T LoadVarying(const VertexData& data,
                const glm::vec3& barycentric_coordinates,
                size_t offset) const {
    const auto stride = data.varyings_stride;
    auto ptr = data.varyings.data() + offset;
    T p1, p2, p3;
    memcpy(&p1, ptr, sizeof(p1));
    ptr += stride;
    memcpy(&p2, ptr, sizeof(p2));
    ptr += stride;
    memcpy(&p3, ptr, sizeof(p3));
    return BarycentricInterpolation(p1, p2, p3, barycentric_coordinates);
  }

  template <class T>
  T LoadVertexData(const VertexData& data, size_t index, size_t offset) const {
    return data.GetVertexData<T>(index, offset);
  }

  template <class T>
  T LoadUniform(const VertexData& data, size_t offset) const {
    T result = {};
    memcpy(&result, data.bindings.uniform.GetData() + offset, sizeof(T));
    return result;
  }

  void ResetMetrics();

  const RasterizerMetrics& GetMetrics() const;

  std::shared_ptr<Image> CaptureDebugDepthTexture() const;

  std::shared_ptr<Image> CaptureDebugStencilTexture() const;

  [[nodiscard]] bool Resize(glm::ivec2 size);

  [[nodiscard]] bool ResizeSamples(SampleCount count);

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

  void DrawTriangle(const VertexData& data);

  void ShadeFragments(const VertexData& data, const Tiler::Data& tiler_data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
