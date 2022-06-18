#pragma once

#include "shader.h"
#include "spirv_cross_shader.h"

extern const struct spirv_cross_interface* ImpellerVertexShader(void);
extern const struct spirv_cross_interface* ImpellerFragmentShader(void);

namespace sft {

class ImpellerShader final : public Shader {
 public:
  ImpellerShader() : fragment_(ImpellerFragmentShader()) {}

  ~ImpellerShader() {}

  glm::vec3 ProcessVertex(glm::vec3 in) override { return in; }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos) override {
    glm::vec4 color;
    ::spirv_cross_set_stage_output(fragment_.Get(), 0, &color, sizeof(color));
    fragment_.Invoke();
    return color;
  }

 private:
  // SPIRVCrossShader vertex_;
  SPIRVCrossShader fragment_;
};

}  // namespace sft
