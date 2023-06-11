/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include "geom.h"
#include "invocation.h"
#include "shader.h"

namespace sft {

class ColorShader final : public Shader {
 public:
  struct VertexData {
    glm::vec3 position;
  };

  struct Uniforms {
    glm::vec4 color;
  };

  struct Varyings {};

  ColorShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec4 ProcessVertex(const VertexInvocation& inv) const override {
    return {VTX(position), 1.0};
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    return UNIFORM(color);
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(ColorShader);
};

}  // namespace sft
