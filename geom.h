#pragma once

#include <cstdint>

namespace sft {

using Scalar = int32_t;
using ScalarF = float;

struct Color {
  const uint32_t color = 0u;

  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : color(a << 24 | r << 16 | g << 8 | b << 0) {}

  constexpr operator uint32_t() const { return color; }
};

constexpr Color kColorRed = {255, 0, 0, 255};
constexpr Color kColorGreen = {0, 255, 0, 255};
constexpr Color kColorBlue = {0, 0, 255, 255};
constexpr Color kColorWhite = {255, 255, 255, 255};
constexpr Color kColorBlack = {0, 0, 0, 255};

struct Point {
  Scalar x = 0;
  Scalar y = 0;

  Point operator+(const Point& o) const { return {x + o.x, y + o.y}; }
  Point operator-(const Point& o) const { return {x - o.x, y - o.y}; }
};

using Vec2 = Point;

struct Vec3F {
  ScalarF x = 0;
  ScalarF y = 0;
  ScalarF z = 0;
};

struct Size {
  Scalar width = 0;
  Scalar height = 0;
};

struct Rect {
  Point origin;
  Size size;
};

}  // namespace sft
