#pragma once

#include <memory>
#include <optional>

#include "geom.h"
#include "shader.h"
#include "vertex_descriptor.h"

namespace sft {

// https://www.w3.org/TR/compositing-1/#porterduffcompositingoperators
enum class BlendMode {
  kClear,
  kCopy,
  kDestination,
  kSourceOver,
  kDestinationOver,
  kSourceIn,
  kDestinationIn,
  kSourceOut,
  kDestinationOut,
  kSourceAtop,
  kDestinationAtop,
  kXOR,
};

enum class CullFace {
  kFront,
  kBack,
};

enum class Winding {
  kClockwise,
  kCounterClockwise,
};

struct Pipeline {
  bool depth_test_enabled = false;
  std::optional<glm::ivec2> viewport;
  std::shared_ptr<Shader> shader;
  std::optional<BlendMode> blend_mode;
  VertexDescriptor vertex_descriptor;
  Winding winding = Winding::kClockwise;
  std::optional<CullFace> cull_face;
  std::optional<Rect> scissor;

  Color Blend(Color p_src, Color p_dst) const {
    auto src = p_src.GetColorF().Premultiply();

    if (!blend_mode.has_value()) {
      return src;
    }

    auto dst = p_dst.GetColorF().Premultiply();

    switch (blend_mode.value()) {
      case BlendMode::kClear:
        return Color(0, 0, 0, 0);
      case BlendMode::kCopy:
        return p_src;
      case BlendMode::kDestination:
        return p_dst;
      case BlendMode::kSourceOver:
        return ColorF{
            src.GetColor() + (1.0f - src.GetAlpha()) * dst.GetColor(),
            src.GetAlpha() + (1.0f - src.GetAlpha()) * dst.GetAlpha(),
        };
        return ColorF{
            src.GetColor() + (1.0f - src.GetAlpha()) * dst.GetColor(),
            src.GetAlpha() + (1.0f - src.GetAlpha()) * dst.GetAlpha(),
        };
      case BlendMode::kDestinationOver:
        return ColorF{
            dst.GetColor() + (1.0f - dst.GetAlpha()) * src.GetColor(),
            dst.GetAlpha() + (1.0f - dst.GetAlpha()) * src.GetAlpha(),
        };
      case BlendMode::kSourceIn:
        return ColorF{
            src.GetColor() * dst.GetColor(),
            src.GetAlpha() * dst.GetAlpha(),
        };
      case BlendMode::kDestinationIn:
        return ColorF{
            dst.GetColor() * src.GetAlpha(),
            src.GetAlpha() * dst.GetAlpha(),
        };
      case BlendMode::kSourceOut:
        return ColorF{
            (1.0f - dst.GetAlpha()) * src.GetColor(),
            (1.0f - dst.GetAlpha()) * src.GetAlpha(),
        };
      case BlendMode::kDestinationOut:
        return ColorF{
            (1.0f - src.GetAlpha()) * dst.GetColor(),
            (1.0f - src.GetAlpha()) * dst.GetAlpha(),
        };
      case BlendMode::kSourceAtop:
        return ColorF{
            dst.GetAlpha() * src.GetColor() +
                (1.0f - src.GetAlpha()) * dst.GetColor(),
            dst.GetAlpha(),
        };
      case BlendMode::kDestinationAtop:
        return ColorF{
            src.GetAlpha() * dst.GetColor() +
                (1.0f - dst.GetAlpha()) * src.GetColor(),
            src.GetAlpha(),
        };
      case BlendMode::kXOR:
        return ColorF{
            (1.0f - dst.GetAlpha()) * src.GetColor() +
                (1.0f - src.GetAlpha()) * dst.GetColor(),
            (1.0f - dst.GetAlpha()) * src.GetAlpha() +
                (1.0f - src.GetAlpha()) * dst.GetAlpha(),
        };
    }
    return kColorBlack;
  }
};

}  // namespace sft
