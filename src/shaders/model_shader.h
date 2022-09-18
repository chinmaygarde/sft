#pragma once

#include "invocation.h"
#include "shader.h"

namespace sft {

class ModelShader final : public Shader {
 public:
  struct VertexData {
    glm::vec3 position;
  };

  struct Uniform {
    glm::mat4 mvp;
  };

  ModelShader() = default;

  glm::vec3 ProcessVertex(
      const VertexInvocation& vertex_invocation) const override {
    const auto mvp =
        vertex_invocation.LoadUniform<glm::mat4>(offsetof(Uniform, mvp));
    auto pos = glm::vec4{vertex_invocation.position, 1.0};
    return pos * mvp;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& fragment_invocation) const override {
    return kColorAqua;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
