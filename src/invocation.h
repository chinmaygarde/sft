#pragma once

#include "buffer.h"
#include "geom.h"
#include "macros.h"

namespace sft {

class Rasterizer;

struct TriangleData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t vertex_id = 0;
  const Buffer& vertex_buffer;

  TriangleData(const Buffer& p_vertex_buffer)
      : vertex_buffer(p_vertex_buffer) {}
};

struct VertexInvocation {
  glm::vec3 position;
  size_t vertex_id;

  VertexInvocation(glm::vec3 p_position, size_t p_vertex_id)
      : position(p_position), vertex_id(p_vertex_id) {}
};

struct FragmentInvocation {
  glm::vec3 barycentric_coordinates;

  glm::vec2 InterpolateVec2(size_t offset) const;

 private:
  friend Rasterizer;

  const Rasterizer& rasterizer;
  const TriangleData& data;

  FragmentInvocation(glm::vec3 p_barycentric_coordinates,
                     const Rasterizer& p_rasterizer,
                     const TriangleData& p_data)
      : barycentric_coordinates(p_barycentric_coordinates),
        rasterizer(p_rasterizer),
        data(p_data) {}
};

}  // namespace sft
