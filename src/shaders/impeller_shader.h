#pragma once

#include <vector>

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
    // Vertex
    ::spirv_cross_set_stage_input(vertex_.Get(), 0, &vertex_position_,
                                  sizeof(vertex_position_));
    ::spirv_cross_set_stage_input(vertex_.Get(), 1, &color_, sizeof(color_));

    ::spirv_cross_set_stage_output(vertex_.Get(), 1, &v_color_,
                                   sizeof(v_color_));

    ::spirv_cross_set_builtin(vertex_.Get(),
                              spirv_cross_builtin::SPIRV_CROSS_BUILTIN_POSITION,
                              &gl_position_, sizeof(gl_position_));

    // Fragment
    ::spirv_cross_set_stage_input(fragment_.Get(), 1, &v_color_,
                                  sizeof(v_color_));

    ::spirv_cross_set_stage_output(fragment_.Get(), 0, &frag_color_,
                                   sizeof(frag_color_));

    vertex_data_.push_back({{1.0, 0.0, 0.0, 1.0}});
    vertex_data_.push_back({{0.0, 1.0, 0.0, 1.0}});
    vertex_data_.push_back({{0.0, 0.0, 1.0, 1.0}});
  }

  ~ImpellerShader() {}

  glm::vec3 ProcessVertex(glm::vec3 in, size_t index) override {
    color_ = vertex_data_[index].color;
    vertex_position_ = in;

    vertex_.Invoke();
    return gl_position_;
  }

  std::optional<Color> ProcessFragment(glm::vec3 bary_pos,
                                       size_t index) override {
    v_color_ = BarycentricInterpolation(
        vertex_data_[index + 0].color, vertex_data_[index + 1].color,
        vertex_data_[index + 2].color, bary_pos);

    fragment_.Invoke();
    return frag_color_;
  }

 private:
  struct PerVertexData {
    glm::vec4 color;
  };
  std::vector<PerVertexData> vertex_data_;

  glm::vec3 vertex_position_;
  glm::vec4 color_;
  glm::vec4 v_color_;
  glm::vec4 gl_position_;
  glm::vec4 frag_color_;
  SPIRVCrossShader vertex_;
  SPIRVCrossShader fragment_;
};

}  // namespace sft
