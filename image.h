#pragma once

#include <algorithm>
#include <cstdlib>
#include "geom.h"

namespace sft {

class Image {
 public:
  Image(size_t p_width, size_t p_height)
      : allocation_(std::calloc(p_width * p_height * 4, sizeof(uint8_t))),
        width(p_width),
        height(p_height) {}

  ~Image() = default;

  void* GetPixels() const { return allocation_; }

  size_t GetWidth() const { return width; }

  size_t GetHeight() const { return height; }

  size_t GetBytesPerPixel() const { return sizeof(uint32_t); }

  bool Set(Scalar x, Scalar y, Color color) {
    if (!allocation_ || x < 0 || y < 0 || x >= width || y >= height) {
      return false;
    }

    auto ptr = reinterpret_cast<uint32_t*>(allocation_) + ((width * y) + x);
    *ptr = color;
    return true;
  }

  void Clear(Color color) {
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        Set(i, j, color);
      }
    }
  }

  static constexpr Scalar Lerp(Scalar min, Scalar max, double factor) {
    return min + ((max - min) * factor);
  }

  void DrawLine(glm::ivec2 p1, glm::ivec2 p2, Color color) {
    const auto steps = std::max(std::abs(p2.x - p1.x), std::abs(p2.y - p1.y));
    for (auto i = 0; i < steps; i++) {
      Set(Lerp(p1.x, p2.x, static_cast<double>(i) / steps),
          Lerp(p1.y, p2.y, static_cast<double>(i) / steps), color);
    }
  }

  void DrawTriangle(glm::ivec2 p1, glm::ivec2 p2, glm::ivec2 p3, Color color) {
    const auto bounding_box = GetBoundingBox(p1, p2, p3);
    for (auto y = 0; y < bounding_box.size.height; y++) {
      for (auto x = 0; x < bounding_box.size.width; x++) {
        const auto p =
            glm::ivec2{x + bounding_box.origin.x, y + bounding_box.origin.y};
        const auto barycentric_coords =
            GetBaryCentricCoordinates(p, p1, p2, p3);
        if (barycentric_coords.x >= 0.0 && barycentric_coords.y >= 0.0 &&
            barycentric_coords.z >= 0.0) {
          Set(p.x, p.y, color);
        }
      }
    }
  }

 private:
  void* allocation_ = nullptr;
  const size_t width;
  const size_t height;

  static Rect GetBoundingBox(glm::ivec2 p1, glm::ivec2 p2, glm::ivec2 p3) {
    const auto min =
        glm::ivec2{std::min({p1.x, p2.x, p3.x}), std::min({p1.y, p2.y, p3.y})};
    const auto max =
        glm::ivec2{std::max({p1.x, p2.x, p3.x}), std::max({p1.y, p2.y, p3.y})};
    return Rect{{min.x, min.y}, {max.x - min.x, max.y - min.y}};
  }

  static glm::vec3 GetBaryCentricCoordinates(glm::ivec2 p,
                                             glm::ivec2 a,
                                             glm::ivec2 b,
                                             glm::ivec2 c) {
    glm::ivec2 v0 = b - a, v1 = c - a, v2 = p - a;
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
