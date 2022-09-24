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
  };

  struct Varyings {
    glm::vec2 texture_coords;
  };

  TextureShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    inv.StoreVarying<glm::vec2>(
        inv.LoadVertexData<glm::vec2>(offsetof(VertexData, texture_coords)),
        offsetof(Varyings, texture_coords));
    return inv.LoadVertexData<glm::vec3>(offsetof(VertexData, position));
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    auto color = texture_->Sample(
        inv.LoadVarying<glm::vec2>(offsetof(Varyings, texture_coords)));
    const auto alpha = inv.LoadUniform<ScalarF>(offsetof(Uniforms, alpha));
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
