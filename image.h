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

  void DrawLine(Point p1, Point p2, Color color) {
    const auto steps = std::max(std::abs(p2.x - p1.x), std::abs(p2.y - p1.y));
    for (uint32_t i = 0; i < steps; i++) {
      Set(Lerp(p1.x, p2.x, static_cast<double>(i) / steps),
          Lerp(p1.y, p2.y, static_cast<double>(i) / steps), color);
    }
  }

  void DrawTriangle(Point p1, Point p2, Point p3, Color color) {
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p1, color);
  }

 private:
  void* allocation_ = nullptr;
  const size_t width;
  const size_t height;

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
};

}  // namespace sft
