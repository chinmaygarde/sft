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
    inv.StoreVarying(
        inv.LoadVertexData<glm::vec4>(offsetof(VertexData, vertex_color)),
        offsetof(Varyings, color));
    inv.StoreVarying(
        inv.LoadVertexData<glm::vec3>(offsetof(VertexData, normal)),
        offsetof(Varyings, normal));
    const auto mvp = inv.LoadUniform<glm::mat4>(offsetof(Uniforms, mvp));
    const auto pos = glm::vec4{
        inv.LoadVertexData<glm::vec3>(offsetof(VertexData, position)), 1.0};
    return pos * mvp;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    auto normal = inv.LoadVarying<glm::vec3>(offsetof(Varyings, normal));
    normal = glm::normalize(normal);
    auto light = inv.LoadUniform<glm::vec3>(offsetof(Uniforms, light));
    auto color = inv.LoadVarying<glm::vec4>(offsetof(Varyings, color));
    auto intensity = glm::dot(normal, light);
    color *= glm::vec4{intensity, intensity, intensity, 1.0};
    return color;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
