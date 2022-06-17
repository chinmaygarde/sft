#pragma once

#include "shader.h"

namespace sft {

class TextureShader final : public Shader {
 public:
  glm::vec3 ProcessVertex(glm::vec3 in) override { return in; }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos) override {
    return Color::FromComponentsF(bary_pos.x, bary_pos.y, bary_pos.z, 1.0);
  }
};

}  // namespace sft
