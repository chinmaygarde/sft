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
    STORE_VARYING(color, LOAD_VERTEX(vertex_color));
    STORE_VARYING(normal, LOAD_VERTEX(normal));
    const auto mvp = LOAD_UNIFORM(mvp);
    const auto pos = glm::vec4{LOAD_VERTEX(position), 1.0};
    return pos * mvp;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    auto normal = glm::normalize(LOAD_VARYING(normal));
    auto light = LOAD_UNIFORM(light);
    auto color = LOAD_VARYING(color);
    auto intensity = glm::dot(normal, light);
    color *= glm::vec4{intensity, intensity, intensity, 1.0};
    return color;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
