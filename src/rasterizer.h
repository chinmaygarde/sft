#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "geom.h"

namespace sft {

class Rasterizer {
 public:
  Rasterizer(glm::ivec2 size);

  ~Rasterizer();

  void* GetPixels() const;

  void* GetDepthPixels() const;

  glm::ivec2 GetSize() const;

  size_t GetBytesPerPixel() const;

  size_t GetDepthBytesPerPixel() const;

  void SetDepthTestsEnabled(bool enabled);

  void Clear(Color color);

  void DrawTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, Color color);

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const glm::ivec2 size_;
  glm::ivec2 viewport_;
  bool depth_test_enabled_ = true;

  void UpdateTexel(Texel texel);

  Rasterizer(const Rasterizer&) = delete;
  Rasterizer& operator=(const Rasterizer&) = delete;
};

}  // namespace sft
