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

  bool Set(glm::vec2 pos, Color color) {
    return Set({pos.x, pos.y, 0.0}, color);
  }

  bool Set(glm::vec3 pos, Color color) {
    if (!color_buffer_ || !depth_buffer_ || pos.x < 0 || pos.y < 0 ||
        pos.x >= width || pos.y >= height) {
      return false;
    }

    glm::ivec3 ipos = pos;

    const auto offset = width * ipos.y + ipos.x;

    auto color_ptr = reinterpret_cast<uint32_t*>(color_buffer_) + offset;
    auto depth_ptr = reinterpret_cast<uint32_t*>(depth_buffer_) + offset;
    *color_ptr = color;
    *depth_ptr = Color::Gray(pos.z);
    return true;
  }

  bool PassesDepthTest(glm::vec3 pos) const {
    if (!color_buffer_ || !depth_buffer_ || pos.x < 0 || pos.y < 0 ||
        pos.x >= width || pos.y >= height) {
      return false;
    }

    auto depth_ptr =
        reinterpret_cast<uint8_t*>(depth_buffer_) +
        ((width * static_cast<uint32_t>(pos.y)) + static_cast<uint32_t>(pos.x));
    const uint8_t current_depth_value = *depth_ptr;
    const uint8_t test_depth_value =
        static_cast<uint8_t>(std::clamp<float>(pos.z, 0.0, 1.0) * 255);
    if (test_depth_value > current_depth_value) {
      std::cout << "Depth fail" << std::endl;
      return false;
    }
    return true;
  }

  void Clear(Color color) {
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        Set({i, j, 0}, color);
      }
    }
  }

  static constexpr Scalar Lerp(Scalar min, Scalar max, double factor) {
    return min + ((max - min) * factor);
  }

  void DrawLine(glm::vec3 p1, glm::vec3 p2, Color color) {
    const auto steps = std::max(std::abs(p2.x - p1.x), std::abs(p2.y - p1.y));
    for (auto i = 0; i < steps; i++) {
      Set(glm::mix(p1, p2, i / steps), color);
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
          Set({p.x, p.y}, color);
        }
      }
    }
  }

 private:
  void* color_buffer_ = nullptr;
  void* depth_buffer_ = nullptr;
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