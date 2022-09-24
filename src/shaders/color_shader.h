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

  struct Uniforms {
    Color color;
  };

  struct Varyings {};

  ColorShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    return VTX(position);
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    return UNIFORM(color);
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ColorShader);
};

}  // namespace sft
