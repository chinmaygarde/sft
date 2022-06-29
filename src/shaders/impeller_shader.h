#pragma once

#include "shader.h"
#include "spirv_cross_shader.h"

extern const struct spirv_cross_interface* impeller_vert_interface(void);
extern const struct spirv_cross_interface* impeller_frag_interface(void);

namespace sft {

class ImpellerShader final : public Shader {
 public:
  ImpellerShader()
      : vertex_(impeller_vert_interface()),
        fragment_(impeller_frag_interface()) {
    ::spirv_cross_set_stage_input(vertex_.Get(), 0, &stage_input_,
                                  sizeof(stage_input_));
    ::spirv_cross_set_builtin(vertex_.Get(),
                              spirv_cross_builtin::SPIRV_CROSS_BUILTIN_POSITION,
                              &gl_position_, sizeof(gl_position_));
    ::spirv_cross_set_stage_output(fragment_.Get(), 0, &frag_color_,
                                   sizeof(frag_color_));
  }

  ~ImpellerShader() {}

  glm::vec3 ProcessVertex(glm::vec3 in) override {
    stage_input_ = in;
    vertex_.Invoke();
    return gl_position_;
  }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos) override {
    fragment_.Invoke();
    return frag_color_;
  }

 private:
  glm::vec3 stage_input_;
  glm::vec4 gl_position_;
  glm::vec4 frag_color_;
  SPIRVCrossShader vertex_;
  SPIRVCrossShader fragment_;
};

}  // namespace sft
