#pragma once

#include "invocation.h"
#include "shader.h"

namespace sft {

class ModelShader final : public Shader {
 public:
  struct VertexData {
    glm::vec4 vertex_color;
    glm::vec3 position;
    glm::vec3 normal;
  };

  struct Uniforms {
    glm::mat4 mvp;
    glm::vec3 light;
  };

  struct Varyings {
    glm::vec4 color;
    glm::vec3 normal;
  };

  ModelShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(vertex_color, color);
    FORWARD(normal, normal);
    const auto mvp = UNIFORM(mvp);
    const auto pos = glm::vec4{VTX(position), 1.0};
    return pos * mvp;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    auto normal = VARYING_LOAD(normal);
    auto light = UNIFORM(light);
    auto intensity = glm::dot(normal, light);
    auto intensity_color = glm::vec4{intensity, intensity, intensity, 1.0};
    auto color = VARYING_LOAD(color);
    return color * intensity_color;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
