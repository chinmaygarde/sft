#pragma once

#include "geom.h"
#include "invocation.h"
#include "shader.h"

namespace sft {

class ColorShader final : public Shader {
 public:
  ColorShader() : ColorShader(kColorBlack) {}

  ColorShader(Color color) : color_(color) {}

  void SetColor(Color color) { color_ = color; }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    return inv.position;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    return color_;
  }

 private:
  Color color_;
};

}  // namespace sft
