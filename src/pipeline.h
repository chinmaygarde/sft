#pragma once

#include <memory>
#include <optional>

#include "attachment.h"
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
  ColorAttachmentDescriptor color_desc;
  DepthAttachmentDescriptor depth_desc;
  StencilAttachmentDescriptor stencil_desc;
  bool depth_test_enabled = false;
  std::optional<glm::ivec2> viewport;
  std::shared_ptr<Shader> shader;
  VertexDescriptor vertex_descriptor;
  Winding winding = Winding::kClockwise;
  std::optional<CullFace> cull_face;
  std::optional<Rect> scissor;
};

}  // namespace sft
