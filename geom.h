#pragma once

#include <cstdint>
#include <cstdlib>

namespace sft {

using Scalar = int32_t;
using ScalarF = float;

struct Color {
  const uint32_t color = 0u;

  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : color(a << 24 | r << 16 | g << 8 | b << 0) {}

  constexpr operator uint32_t() const { return color; }

  static Color Random() {
    return Color{static_cast<uint8_t>(std::rand() % 255),
                 static_cast<uint8_t>(std::rand() % 255),
                 static_cast<uint8_t>(std::rand() % 255), 255};
  }
};

constexpr Color kColorRed = {255, 0, 0, 255};
constexpr Color kColorGreen = {0, 255, 0, 255};
constexpr Color kColorBlue = {0, 0, 255, 255};
constexpr Color kColorWhite = {255, 255, 255, 255};
constexpr Color kColorBlack = {0, 0, 0, 255};

struct Vec3F {
  ScalarF x = 0;
  ScalarF y = 0;
  ScalarF z = 0;
};

struct Point {
  Scalar x = 0;
  Scalar y = 0;

  Point(Scalar p_x, Scalar p_y) : x(p_x), y(p_y) {}

  Point(const Vec3F& v) : x(v.x), y(v.y) {}

  Point operator+(const Point& o) const { return {x + o.x, y + o.y}; }
  Point operator-(const Point& o) const { return {x - o.x, y - o.y}; }
};

using Vec2 = Point;

struct Size {
  Scalar width = 0;
  Scalar height = 0;
};

struct Rect {
  Point origin;
  Size size;
};

}  // namespace sft
