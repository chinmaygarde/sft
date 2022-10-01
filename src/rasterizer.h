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
#include "renderer.h"
#include "triangle_data.h"

namespace sft {

class Rasterizer final : public Renderer {
 public:
  Rasterizer(glm::ivec2 size);

  // |Renderer|
  ~Rasterizer() override;

  // |Renderer|
  void* GetPixels() const override;

  // |Renderer|
  glm::ivec2 GetSize() const override;

  // |Renderer|
  size_t GetBytesPerPixel() const override;

  void Clear(Color color);

  void Draw(const Pipeline& pipeline,
            const BufferView& vertex_buffer,
            const BufferView& uniform_buffer,
            size_t count);

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
    const auto vtx_desc = data.pipeline.vertex_descriptor;
    const uint8_t* vtx_ptr = data.vertex_buffer.GetData();
    vtx_ptr += (vtx_desc.stride * index);
    vtx_ptr += offset;
    T result = {};
    memcpy(&result, vtx_ptr, sizeof(result));
    return result;
  }

  template <class T>
  T LoadUniform(const TriangleData& data, size_t offset) const {
    T result = {};
    memcpy(&result, data.uniform_buffer.GetData() + offset, sizeof(T));
    return result;
  }

  void ResetMetrics();

  const RasterizerMetrics& GetMetrics() const;

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const glm::ivec2 size_;
  RasterizerMetrics metrics_;

  bool FragmentPassesDepthTest(const Pipeline& pipeline,
                               glm::ivec2 pos,
                               ScalarF depth) const;

  void UpdateTexel(const Pipeline& pipeline, Texel texel);

  void DrawTriangle(const TriangleData& data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
