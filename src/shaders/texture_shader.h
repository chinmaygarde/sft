#pragma once

#include "shader.h"
#include "texture.h"

namespace sft {

class TextureShader final : public Shader {
 public:
  glm::vec3 ProcessVertex(glm::vec3 in) override { return in; }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos) override {
    return texture_->Sample({bary_pos.x, bary_pos.y});
  }

  void SetTexture(std::shared_ptr<Texture> texture) {
    texture_ = std::move(texture);
  }

 private:
  std::shared_ptr<Texture> texture_;
};

}  // namespace sft
