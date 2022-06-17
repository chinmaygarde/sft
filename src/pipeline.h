#pragma once

#include <memory>

#include "geom.h"
#include "shader.h"

namespace sft {

struct Pipeline {
  bool depth_test_enabled = true;
  glm::ivec2 viewport;
  std::shared_ptr<Shader> shader;
};

}  // namespace sft
