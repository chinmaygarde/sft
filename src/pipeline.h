#pragma once

#include <memory>
#include <optional>

#include "blend.h"
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
  BlendDescriptor blend;
  VertexDescriptor vertex_descriptor;
  Winding winding = Winding::kClockwise;
  std::optional<CullFace> cull_face;
  std::optional<Rect> scissor;
};

}  // namespace sft
