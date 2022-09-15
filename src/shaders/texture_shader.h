#pragma once

#include <vector>

#include "macros.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class TextureShader final : public Shader {
 public:
  TextureShader() = default;

  glm::vec3 ProcessVertex(glm::vec3 in, size_t index) override { return in; }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos,
                                       size_t index) override {
    SFT_ASSERT(texture_ && "Texture is present.");
    return texture_->Sample({bary_pos.x, bary_pos.y});
  }

  void SetTexture(std::shared_ptr<Texture> texture) {
    texture_ = std::move(texture);
  }

 private:
  std::shared_ptr<Texture> texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(TextureShader);
};

}  // namespace sft
