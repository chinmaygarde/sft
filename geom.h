#pragma once

#include <cstdint>

namespace sft {

using Scalar = int32_t;

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
};

struct Size {
  Scalar width = 0;
  Scalar height = 0;
};

}  // namespace sft
