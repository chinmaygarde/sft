#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "buffer_view.h"
#include "framebuffer.h"
#include "geom.h"
#include "pipeline.h"
#include "rasterizer_metrics.h"
#include "renderer.h"
#include "triangle_data.h"

namespace sft {

class Texture;

class Rasterizer final : public Renderer {
 public:
  Rasterizer(glm::ivec2 size);

  // |Renderer|
  ~Rasterizer() override;

  // |Renderer|
  const void* GetPixels() const override;

  // |Renderer|
  glm::ivec2 GetSize() const override;

  // |Renderer|
  size_t GetBytesPerPixel() const override;

  void Clear(Color color);

  void Draw(const Pipeline& pipeline,
            const BufferView& vertex_buffer,
            const BufferView& uniform_buffer,
            size_t count,
            uint32_t stencil_refernece = 0) {
    return Draw(pipeline, vertex_buffer, {}, uniform_buffer, count,
                stencil_refernece);
  }

  void Draw(const Pipeline& pipeline,
            const BufferView& vertex_buffer,
            const BufferView& index_buffer,
            const BufferView& uniform_buffer,
            size_t count,
            uint32_t stencil_reference = 0) {
    metrics_.draw_count++;
    const auto varyings_size = pipeline.shader->GetVaryingsSize();
    auto* varyings = reinterpret_cast<uint8_t*>(::alloca(varyings_size * 3u));
    TriangleData data(pipeline,          //
                      vertex_buffer,     //
                      index_buffer,      //
                      uniform_buffer,    //
                      varyings_size,     //
                      varyings,          //
                      stencil_reference  //
    );
    const auto vtx_offset = pipeline.vertex_descriptor.offset;
    for (size_t i = 0; i < count; i += 3) {
      data.base_vertex_id = i;
      data.p1 = data.GetVertexData<decltype(data.p1)>(i + 0, vtx_offset);
      data.p2 = data.GetVertexData<decltype(data.p2)>(i + 1, vtx_offset);
      data.p3 = data.GetVertexData<decltype(data.p3)>(i + 2, vtx_offset);
      DrawTriangle(data);
    }
  }

  template <class T>
  void StoreVarying(const TriangleData& data,
                    const T& val,
                    size_t index,
                    size_t offset) const {
    auto* ptr = data.varyings;
    ptr += offset;
    ptr += (data.varyings_stride * (index % 3));
    memcpy(ptr, &val, sizeof(T));
  }

  template <class T>
  T LoadVarying(const TriangleData& data,
                const glm::vec3& barycentric_coordinates,
                size_t offset) const {
    const auto stride = data.varyings_stride;
    auto ptr = data.varyings + offset;
    T p1, p2, p3;
    memcpy(&p1, ptr, sizeof(p1));
    ptr += stride;
    memcpy(&p2, ptr, sizeof(p2));
    ptr += stride;
    memcpy(&p3, ptr, sizeof(p3));
    return BarycentricInterpolation(p1, p2, p3, barycentric_coordinates);
  }

  template <class T>
  T LoadVertexData(const TriangleData& data,
                   size_t index,
                   size_t offset) const {
    return data.GetVertexData<T>(index, offset);
  }

  template <class T>
  T LoadUniform(const TriangleData& data, size_t offset) const {
    T result = {};
    memcpy(&result, data.uniform_buffer.GetData() + offset, sizeof(T));
    return result;
  }

  void ResetMetrics();

  const RasterizerMetrics& GetMetrics() const;

  std::shared_ptr<Texture> CaptureDebugDepthTexture() const;

 private:
  Framebuffer<Color> color0_;
  Framebuffer<ScalarF> depth0_;
  Framebuffer<uint32_t> stencil0_;
  const glm::ivec2 size_;
  RasterizerMetrics metrics_;

  bool FragmentPassesDepthTest(const Pipeline& pipeline,
                               glm::ivec2 pos,
                               ScalarF depth) const;

  bool FragmentPassesStencilTest(const Pipeline& pipeline,
                                 glm::ivec2 pos,
                                 uint32_t reference_value) const;

  void UpdateTexel(const Pipeline& pipeline, Texel texel);

  void DrawTriangle(const TriangleData& data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
