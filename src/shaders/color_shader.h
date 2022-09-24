#pragma once

#include "geom.h"
#include "invocation.h"
#include "shader.h"

namespace sft {

class ColorShader final : public Shader {
 public:
  struct VertexData {
    glm::vec3 position;
  };

  struct Uniforms {};

  struct Varyings {};

  ColorShader() : ColorShader(kColorBlack) {}

  ColorShader(Color color) : color_(color) {}

  void SetColor(Color color) { color_ = color; }

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    return inv.LoadUniform<glm::vec3>(offsetof(VertexData, position));
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    return color_;
  }

 private:
  Color color_;
};

}  // namespace sft
