#pragma once

#include "invocation.h"
#include "shader.h"

namespace sft {

class ModelShader final : public Shader {
 public:
  struct VertexData {
    glm::vec4 vertex_color;
    glm::vec3 position;
  };

  struct Uniform {
    glm::mat4 mvp;
  };

  struct Varyings {
    glm::vec4 color;
  };

  ModelShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    inv.StoreVarying(inv.LoadVertexData(offsetof(VertexData, vertex_color)),
                     offsetof(Varyings, color));
    const auto mvp = inv.LoadUniform<glm::mat4>(offsetof(Uniform, mvp));
    const auto pos =
        glm::vec4{inv.LoadVertexData(offsetof(VertexData, position)), 1.0};
    return pos * mvp;
  }

  std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const override {
    return kColorBeige;
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ModelShader);
};

}  // namespace sft
