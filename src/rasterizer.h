#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "geom.h"
#include "pipeline.h"
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
            const Buffer& vertex_buffer,
            const Buffer& uniform_buffer,
            size_t count) {
    Buffer index_buffer;
    for (size_t i = 0; i < count; i++) {
      index_buffer.Emplace<uint32_t>(i);
    }
    Draw(pipeline, vertex_buffer, index_buffer, uniform_buffer, count);
  }

  void Draw(const Pipeline& pipeline,
            const Buffer& vertex_buffer,
            const Buffer& index_buffer,
            const Buffer& uniform_buffer,
            size_t count) {
    const auto& vtx_desc = pipeline.vertex_descriptor;
    const auto* vtx_ptr = reinterpret_cast<const glm::vec3*>(
        vertex_buffer.GetData() + vtx_desc.offset);
    const auto* idx_ptr =
        reinterpret_cast<const uint32_t*>(index_buffer.GetData());
    const auto varyings_size = pipeline.shader->GetVaryingsSize();
    auto* varyings = reinterpret_cast<uint8_t*>(::alloca(varyings_size * 3u));
    TriangleData data(pipeline,        //
                      vertex_buffer,   //
                      uniform_buffer,  //
                      varyings_size,   //
                      varyings         //
    );
    for (size_t i = 0; i < count; i += 3) {
      data.base_vertex_id = i;
      size_t index = idx_ptr[i];
      memcpy(&data.p1, &vtx_ptr[index + 0], sizeof(glm::vec3));
      memcpy(&data.p2, &vtx_ptr[index + 1], sizeof(glm::vec3));
      memcpy(&data.p3, &vtx_ptr[index + 2], sizeof(glm::vec3));
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

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const glm::ivec2 size_;

  bool FragmentPassesDepthTest(const Pipeline& pipeline,
                               glm::ivec2 pos,
                               ScalarF depth) const;

  void UpdateTexel(const Pipeline& pipeline, Texel texel);

  void DrawTriangle(const TriangleData& data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
