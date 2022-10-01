#pragma once

#include "geom.h"
#include "macros.h"

namespace sft {

class Buffer;

struct Pipeline;
struct BufferView;

struct TriangleData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t base_vertex_id = 0;
  const Pipeline& pipeline;
  const BufferView& vertex_buffer;
  const BufferView& uniform_buffer;
  const size_t varyings_stride;
  uint8_t* varyings = nullptr;

  TriangleData(const Pipeline& p_pipeline,
               const BufferView& p_vertex_buffer,
               const BufferView& p_uniform_buffer,
               size_t p_varyings_stride,
               uint8_t* p_varyings)
      : pipeline(p_pipeline),
        vertex_buffer(p_vertex_buffer),
        uniform_buffer(p_uniform_buffer),
        varyings_stride(p_varyings_stride),
        varyings(p_varyings) {}
};

}  // namespace sft
