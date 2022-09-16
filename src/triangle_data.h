#pragma once

#include "geom.h"
#include "macros.h"

namespace sft {

class Buffer;

struct TriangleData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t vertex_id = 0;
  const Buffer& vertex_buffer;

  TriangleData(const Buffer& p_vertex_buffer)
      : vertex_buffer(p_vertex_buffer) {}
};

}  // namespace sft
