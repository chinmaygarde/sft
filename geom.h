#pragma once

#include <cstdint>

namespace sft {

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
  int32_t x = 0;
  int32_t y = 0;
};

}  // namespace sft
