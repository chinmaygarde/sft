#pragma once

#include "geom.h"
#include "shader.h"

namespace sft {

class ColorShader final : public Shader {
 public:
  ColorShader() : color_(kColorBlack) {}

  void SetColor(Color color) { color_ = color; }

  glm::vec3 ProcessVertex(glm::vec3 in, size_t index) override { return in; }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos) override {
    return color_;
  }

 private:
  Color color_;
};

}  // namespace sft
