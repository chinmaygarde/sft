#pragma once

#include <memory>
#include <optional>

#include "blend.h"
#include "geom.h"
#include "shader.h"
#include "vertex_descriptor.h"

namespace sft {

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
  BlendDescriptor blend;
  VertexDescriptor vertex_descriptor;
  Winding winding = Winding::kClockwise;
  std::optional<CullFace> cull_face;
  std::optional<Rect> scissor;
};

}  // namespace sft
