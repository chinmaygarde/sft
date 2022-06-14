#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "geom.h"

namespace sft {

class Image {
 public:
  Image(glm::ivec2 size);

  ~Image();

  void* GetPixels() const;

  void* GetDepthPixels() const;

  size_t GetWidth() const;

  size_t GetHeight() const;

  size_t GetBytesPerPixel() const;

  size_t GetDepthBytesPerPixel() const;

  void SetDepthTestsEnabled(bool enabled);

  bool Set(glm::vec3 pos, Color color);

  void Clear(Color color);

  void DrawLine(glm::vec3 p1, glm::vec3 p2, Color color);

  void DrawTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, Color color);

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const size_t width;
  const size_t height;
  bool depth_test_enabled_ = true;

  static Rect GetBoundingBox(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

  static glm::vec3 GetBaryCentricCoordinates(glm::vec2 p,
                                             glm::vec2 a,
                                             glm::vec2 b,
                                             glm::vec2 c);

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
};

}  // namespace sft
