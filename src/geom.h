#pragma once

#include <cstdint>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace sft {

using Scalar = int32_t;
using ScalarF = float;

struct ColorF {
  ScalarF red = 0.0;
  ScalarF green = 0.0;
  ScalarF blue = 0.0;
  ScalarF alpha = 0.0;

  constexpr ColorF() = default;

  constexpr ColorF(ScalarF p_red,
                   ScalarF p_green,
                   ScalarF p_blue,
                   ScalarF p_alpha)
      : red(p_red), green(p_green), blue(p_blue), alpha(p_alpha) {}

  constexpr ColorF(glm::vec3 color, ScalarF p_alpha)
      : red(color.r), green(color.g), blue(color.b), alpha(p_alpha) {}

  constexpr glm::vec3 GetColor() const { return {red, green, blue}; }

  constexpr ScalarF GetAlpha() const { return alpha; };

  constexpr ColorF Premultiply() const {
    return {red * alpha, green * alpha, blue * alpha, alpha};
  }
};

struct Color {
  uint32_t color = 0u;

  Color() = default;

  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : color(a << 24 | r << 16 | g << 8 | b << 0) {}

  constexpr Color(const glm::vec4& c)
      : Color(255 * c.r, 255 * c.g, 255 * c.b, 255 * c.a) {}

  constexpr Color(uint32_t p_color) : color(p_color) {}

  constexpr Color(ColorF c)
      : Color(255 * c.red, 255 * c.green, 255 * c.blue, 255 * c.alpha) {}

  constexpr operator uint32_t() const { return color; }

  constexpr uint8_t GetRed() const { return color >> 16; }

  constexpr uint8_t GetGreen() const { return color >> 8; }

  constexpr uint8_t GetBlue() const { return color >> 0; }

  constexpr uint8_t GetAlpha() const { return color >> 24; }

  constexpr Color WithAlpha(uint8_t alpha) const {
    return Color{GetRed(), GetGreen(), GetBlue(), alpha};
  }

  constexpr ColorF GetColorF() const {
    return {
        GetRed() / 255.0f,    //
        GetGreen() / 255.0f,  //
        GetBlue() / 255.0f,   //
        GetAlpha() / 255.0f   //
    };
  }

  static Color FromComponentsF(ScalarF r, ScalarF g, ScalarF b, ScalarF a) {
    return Color{
        static_cast<uint8_t>(255 * r),
        static_cast<uint8_t>(255 * g),
        static_cast<uint8_t>(255 * b),
        static_cast<uint8_t>(255 * a),
    };
  }

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
constexpr Color kColorTransparentBlack = {0, 0, 0, 0};

struct Size {
  ScalarF width = 0.0;
  ScalarF height = 0.0;
};

struct Rect {
  glm::vec2 origin;
  Size size;
};

struct Texel {
  glm::ivec2 pos;
  ScalarF depth = 0.0;
  Color color;
};

struct Sphere {
  glm::vec3 center;
  ScalarF radius = 0.0;

  bool RayIntersects(glm::vec3 origin, glm::vec3 direction) const {
    return true;
  }
};

}  // namespace sft
