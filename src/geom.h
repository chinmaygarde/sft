#pragma once

#include <cstdint>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace sft {

using Scalar = int32_t;
using ScalarF = float;

struct Color {
  const uint32_t color = 0u;

  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : color(a << 24 | r << 16 | g << 8 | b << 0) {}

  constexpr Color(uint32_t p_color) : color(p_color) {}

  constexpr operator uint32_t() const { return color; }

  constexpr uint8_t GetRed() const { return color >> 16; }

  constexpr uint8_t GetGreen() const { return color >> 8; }

  constexpr uint8_t GetBlue() const { return color >> 0; }

  constexpr uint8_t GetAlpha() const { return color >> 24; }

  static Color Random() {
    return Color{static_cast<uint8_t>(std::rand() % 255),
                 static_cast<uint8_t>(std::rand() % 255),
                 static_cast<uint8_t>(std::rand() % 255), 255};
  }

  static Color Gray(ScalarF gray) {
    const uint8_t g = 255 * glm::clamp<ScalarF>(gray, 0.0, 1.0);
    return Color{g, g, g, 255};
  }
};

constexpr Color kColorRed = {255, 0, 0, 255};
constexpr Color kColorGreen = {0, 255, 0, 255};
constexpr Color kColorBlue = {0, 0, 255, 255};
constexpr Color kColorWhite = {255, 255, 255, 255};
constexpr Color kColorBlack = {0, 0, 0, 255};

struct Size {
  ScalarF width = 0.0;
  ScalarF height = 0.0;
};

struct Rect {
  glm::vec2 origin;
  Size size;
};

}  // namespace sft
