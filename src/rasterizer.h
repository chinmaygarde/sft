#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "geom.h"
#include "invocation.h"
#include "pipeline.h"
#include "renderer.h"

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

  glm::vec2 InterpolateVec2(const TriangleData& data,
                            const glm::vec3& barycentric_coordinates,
                            size_t offset) const;

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
