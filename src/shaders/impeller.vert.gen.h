// This C++ shader is autogenerated by spirv-cross.
#include <stdint.h>
#include <array>
#include "spirv_cross/external_interface.h"
#include "spirv_cross/internal_interface.hpp"

using namespace spirv_cross;
using namespace glm;

namespace Impl {
struct Shader {
  struct Resources : VertexResources {
    struct FrameInfo {
      mat4 mvp;
    };

    internal::Resource<FrameInfo> frame_info__;
#define frame_info __res->frame_info__.get()

    internal::StageOutput<vec2> v_screen_position__;
#define v_screen_position __res->v_screen_position__.get()

    internal::StageInput<vec2> vertex_position__;
#define vertex_position __res->vertex_position__.get()

    inline void init(spirv_cross_shader& s) {
      VertexResources::init(s);
      s.register_resource(frame_info__, 0, 0);
      s.register_stage_output(v_screen_position__, 0);
      s.register_stage_input(vertex_position__, 0);
    }
  };

  Resources* __res;

  inline void main() {
    v_screen_position = vertex_position;
    gl_Position = frame_info.mvp * vec4(vertex_position, 0.0f, 1.0f);
  }
};
}  // namespace Impl

spirv_cross_shader_t* spirv_cross_construct(void) {
  return new VertexShader<Impl::Shader, Impl::Shader::Resources>();
}

void spirv_cross_destruct(spirv_cross_shader_t* shader) {
  delete static_cast<VertexShader<Impl::Shader, Impl::Shader::Resources>*>(
      shader);
}

void spirv_cross_invoke(spirv_cross_shader_t* shader) {
  static_cast<VertexShader<Impl::Shader, Impl::Shader::Resources>*>(shader)
      ->invoke();
}

static const struct spirv_cross_interface vtable = {
    spirv_cross_construct,
    spirv_cross_destruct,
    spirv_cross_invoke,
};

const struct spirv_cross_interface* spirv_cross_get_interface(void) {
  return &vtable;
}
