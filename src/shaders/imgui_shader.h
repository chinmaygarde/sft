#pragma once

#include "invocation.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class ImGuiShader final : public Shader {
 public:
  struct VertexData {
    glm::vec2 vertex_position;
    glm::vec2 texture_coordinates;
    glm::vec4 vertex_color;
  };

  struct Uniforms {
    glm::mat4 mvp;
  };

  struct Varyings {
    glm::vec2 texture_coordinates;
    glm::vec4 vertex_color;
  };

  ImGuiShader() = default;

  ~ImGuiShader() = default;

  void SetTexture(std::shared_ptr<Texture> tex) { texture_ = std::move(tex); }

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(texture_coordinates, texture_coordinates);
    FORWARD(vertex_color, vertex_color);
    auto pos = glm::vec4{VTX(vertex_position), 0.0, 1.0};
    auto mvp = UNIFORM(mvp);
    return mvp * pos;
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    const glm::vec4 texture_color =
        texture_ ? texture_->Sample(VARYING_LOAD(texture_coordinates))
                 : glm::vec4{1.0, 1.0, 1.0, 1.0};
    const glm::vec4 color = VARYING_LOAD(vertex_color);
    return color * texture_color;
  }

 private:
  std::shared_ptr<Texture> texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(ImGuiShader);
};

}  // namespace sft
