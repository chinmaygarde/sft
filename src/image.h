#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "geom.h"

namespace sft {

class Image {
 public:
  Image(glm::ivec2 size)
      : color_buffer_(std::calloc(size.x * size.y * 4, sizeof(uint8_t))),
        depth_buffer_(std::calloc(size.x * size.y * 4, sizeof(uint8_t))),
        width(size.x),
        height(size.y) {}

  ~Image() {
    std::free(depth_buffer_);
    std::free(color_buffer_);
  }

  void* GetPixels() const { return color_buffer_; }

  void* GetDepthPixels() const { return depth_buffer_; }

  size_t GetWidth() const { return width; }

  size_t GetHeight() const { return height; }

  size_t GetBytesPerPixel() const { return sizeof(uint32_t); }

  size_t GetDepthBytesPerPixel() const { return sizeof(uint32_t); }

  void SetDepthTestsEnabled(bool enabled) { depth_test_enabled_ = enabled; }

  bool Set(glm::vec3 pos, Color color) {
    if (!color_buffer_ || !depth_buffer_ || pos.x < 0 || pos.y < 0 ||
        pos.x >= width || pos.y >= height) {
      return false;
    }

    const glm::ivec2 ipos = pos;

    const auto offset = width * ipos.y + ipos.x;

    auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;
    auto depth_ptr = reinterpret_cast<uint32_t*>(depth_buffer_) + offset;

    auto new_depth = Color::Gray(pos.z);

    if (depth_test_enabled_) {
      const auto old_depth = Color(depth_ptr[0]);
      if (new_depth.GetRed() < old_depth.GetRed()) {
        return true;
      }
    }

    *color_ptr = color;
    *depth_ptr = new_depth;
    return true;
  }

  void Clear(Color color) {
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        Set({i, j, 0}, color);
      }
    }
  }

  void DrawLine(glm::vec3 p1, glm::vec3 p2, Color color) {
    const auto steps = std::max(std::abs(p2.x - p1.x), std::abs(p2.y - p1.y));
    for (auto i = 0; i < steps; i++) {
      Set(glm::mix(p1, p2, i / steps), color);
    }
  }

  void DrawTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, Color color) {
    const auto bounding_box = GetBoundingBox(p1, p2, p3);
    for (auto y = 0; y < bounding_box.size.height; y++) {
      for (auto x = 0; x < bounding_box.size.width; x++) {
        const auto p =
            glm::vec2{x + bounding_box.origin.x, y + bounding_box.origin.y};
        const auto bary = GetBaryCentricCoordinates(p, p1, p2, p3);
        if (bary.x >= 0.0 && bary.y >= 0.0 && bary.z >= 0.0) {
          const auto bary_pos = (bary.x * p1 + bary.y * p2 + bary.z * p3);
          Set({p.x, p.y, bary_pos.z}, color);
        }
      }
    }
  }

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
  const size_t width;
  const size_t height;
  bool depth_test_enabled_ = true;

  static Rect GetBoundingBox(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    const auto min =
        glm::vec2{std::min({p1.x, p2.x, p3.x}), std::min({p1.y, p2.y, p3.y})};
    const auto max =
        glm::vec2{std::max({p1.x, p2.x, p3.x}), std::max({p1.y, p2.y, p3.y})};
    return Rect{{min.x, min.y}, {max.x - min.x, max.y - min.y}};
  }

  static glm::vec3 GetBaryCentricCoordinates(glm::vec2 p,
                                             glm::vec2 a,
                                             glm::vec2 b,
                                             glm::vec2 c) {
    glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    ScalarF one_over_den = 1.0f / (v0.x * v1.y - v1.x * v0.y);
    ScalarF v = (v2.x * v1.y - v1.x * v2.y) * one_over_den;
    ScalarF w = (v0.x * v2.y - v2.x * v0.y) * one_over_den;
    ScalarF u = 1.0f - v - w;
    return {u, v, w};
  }

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
};

}  // namespace sft
