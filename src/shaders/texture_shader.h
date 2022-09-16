#pragma once

#include <vector>

#include "invocation.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class TextureShader final : public Shader {
 public:
  struct VertexDescription {
    glm::vec2 texture_coords;
    glm::vec3 position;
  };

  TextureShader() = default;

  glm::vec3 ProcessVertex(const VertexInvocation& inv) override {
    return inv.position;
  }

  std::optional<Color> ProcessFragment(const FragmentInvocation& inv) override {
    return texture_->Sample(
        inv.InterpolateVec2(offsetof(VertexDescription, texture_coords)));
  }

  void SetTexture(std::shared_ptr<Texture> texture) {
    texture_ = std::move(texture);
  }

 private:
  std::shared_ptr<Texture> texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(TextureShader);
};

}  // namespace sft
