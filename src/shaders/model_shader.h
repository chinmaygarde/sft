#pragma once

#include "invocation.h"
#include "shader.h"

namespace sft {

class ModelShader final : public Shader {
 public:
  struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
  };

  struct Uniforms {
    glm::mat4 mvp;
    glm::vec3 light;
    glm::vec4 color;
  };

  struct Varyings {
    glm::vec3 normal;
  };

  ModelShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(normal, normal);
    const auto mvp = UNIFORM(mvp);
    const auto pos = glm::vec4{VTX(position), 1.0};
    return mvp * pos;
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    auto mvp = glm::identity<glm::mat4>();
    auto normal = mvp * glm::vec4{VARYING_LOAD(normal), 1.0};
    auto light = mvp * glm::vec4{UNIFORM(light), 1.0};
    normal = glm::normalize(normal);
    light = glm::normalize(light);
    auto intensity = glm::dot(light, normal);
    auto intensity_color = glm::vec4{intensity, intensity, intensity, 1.0};
    auto color = UNIFORM(color) * intensity_color;
    return color;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
