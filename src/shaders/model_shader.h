#pragma once

#include "invocation.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class ModelShader final : public Shader {
 public:
  struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coord;
  };

  struct Uniforms {
    glm::mat4 mvp;
    glm::vec3 light;
    glm::vec4 color;
  };

  struct Varyings {
    glm::vec2 texture_coord;
    glm::vec3 normal;
  };

  ModelShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(normal, normal);
    FORWARD(texture_coord, texture_coord);
    const auto mvp = UNIFORM(mvp);
    const auto pos = glm::vec4{VTX(position), 1.0};
    return mvp * pos;
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    auto normal = glm::vec4{VARYING_LOAD(normal), 1.0};
    auto light = glm::vec4{UNIFORM(light), 1.0};
    normal = glm::normalize(normal);
    light = glm::normalize(light);
    auto intensity = glm::dot(light, normal);
    auto intensity_color = glm::vec4{intensity, intensity, intensity, 1.0};
    glm::vec4 color;
    if (texture_) {
      color = texture_->Sample(VARYING_LOAD(texture_coord));
    } else {
      color = UNIFORM(color);
    }
    color *= intensity_color;
    return color;
  }

  void SetTexture(std::shared_ptr<Texture> texture) {
    texture_ = std::move(texture);
  }

 private:
  std::shared_ptr<Texture> texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
