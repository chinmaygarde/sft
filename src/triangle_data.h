#pragma once

#include "geom.h"
#include "macros.h"

namespace sft {

class Buffer;
struct Pipeline;

struct TriangleData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t vertex_id = 0;
  const Pipeline& pipeline;
  const Buffer& vertex_buffer;
  const Buffer& uniform_buffer;

  TriangleData(const Pipeline& p_pipeline,
               const Buffer& p_vertex_buffer,
               const Buffer& p_uniform_buffer)
      : pipeline(p_pipeline),
        vertex_buffer(p_vertex_buffer),
        uniform_buffer(p_uniform_buffer) {}
};

}  // namespace sft
