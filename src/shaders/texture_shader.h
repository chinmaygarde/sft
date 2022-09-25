#pragma once

#include <vector>

#include "invocation.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class TextureShader final : public Shader {
 public:
  struct VertexData {
    glm::vec2 texture_coords;
    glm::vec3 position;
  };

  struct Uniforms {
    ScalarF alpha;
    glm::vec2 offset;
  };

  struct Varyings {
    glm::vec2 texture_coords;
  };

  TextureShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(texture_coords, texture_coords);
    auto position = VTX(position);
    auto offset = UNIFORM(offset);
    position.x += offset.x;
    position.y += offset.y;
    return position;
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    auto color = texture_->Sample(VARYING_LOAD(texture_coords));
    const auto alpha = glm::clamp(UNIFORM(alpha), 0.0f, 1.0f);
    color.a *= alpha;
    return color;
  }

  void SetTexture(std::shared_ptr<Texture> texture) {
    texture_ = std::move(texture);
  }

 private:
  std::shared_ptr<Texture> texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(TextureShader);
};

}  // namespace sft
