#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "geom.h"
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

  void SetPipeline(std::shared_ptr<Pipeline> pipeline) { pipeline_ = pipeline; }

  void Clear(Color color);

  void DrawTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const glm::ivec2 size_;
  std::shared_ptr<Pipeline> pipeline_;

  void UpdateTexel(Texel texel);

  Rasterizer(const Rasterizer&) = delete;
  Rasterizer& operator=(const Rasterizer&) = delete;
};

}  // namespace sft
