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
  void* GetDepthPixels() const override;

  // |Renderer|
  glm::ivec2 GetSize() const override;

  // |Renderer|
  size_t GetBytesPerPixel() const override;

  // |Renderer|
  size_t GetDepthBytesPerPixel() const override;

  void SetPipeline(std::shared_ptr<Pipeline> pipeline);

  void Clear(Color color);

  void Draw(const Buffer& vertex_buffer, size_t count);

  template <class T>
  T Interpolate(const TriangleData& data,
                const glm::vec3& barycentric_coordinates,
                size_t offset) const {
    const auto& vtx_desc = pipeline_->vertex_descriptor;
    const uint8_t* vtx_ptr = data.vertex_buffer.GetData() +
                             (vtx_desc.stride * data.vertex_id) + offset;
    T p1, p2, p3;
    memcpy(&p1, vtx_ptr, sizeof(p1));
    vtx_ptr += vtx_desc.stride;
    memcpy(&p2, vtx_ptr, sizeof(p2));
    vtx_ptr += vtx_desc.stride;
    memcpy(&p3, vtx_ptr, sizeof(p3));
    vtx_ptr += vtx_desc.stride;
    return BarycentricInterpolation(p1, p2, p3, barycentric_coordinates);
  }

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const glm::ivec2 size_;
  std::shared_ptr<Pipeline> pipeline_;

  void UpdateTexel(Texel texel);

  void DrawTriangle(const TriangleData& data);

  Rasterizer(const Rasterizer&) = delete;
  Rasterizer& operator=(const Rasterizer&) = delete;
};

}  // namespace sft
