#pragma once

#include <type_traits>

#include "buffer_view.h"
#include "geom.h"
#include "macros.h"
#include "pipeline.h"

namespace sft {

class Buffer;

struct BufferView;

struct TriangleData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t base_vertex_id = 0;
  const Pipeline& pipeline;
  const BufferView& vertex_buffer;
  const BufferView& index_buffer;
  const BufferView& uniform_buffer;
  const size_t varyings_stride;
  uint8_t* varyings = nullptr;

  TriangleData(const Pipeline& p_pipeline,
               const BufferView& p_vertex_buffer,
               const BufferView& p_index_buffer,
               const BufferView& p_uniform_buffer,
               size_t p_varyings_stride,
               uint8_t* p_varyings)
      : pipeline(p_pipeline),
        vertex_buffer(p_vertex_buffer),
        index_buffer(p_index_buffer),
        uniform_buffer(p_uniform_buffer),
        varyings_stride(p_varyings_stride),
        varyings(p_varyings) {}

  constexpr size_t GetVertexIndex(size_t index) const {
    if (!index_buffer) {
      return index;
    }
    switch (pipeline.vertex_descriptor.index_type) {
      case IndexType::kUInt32: {
        auto index_ptr =
            reinterpret_cast<const uint32_t*>(index_buffer.GetData()) + index;
        return *index_ptr;
      }
      case IndexType::kUInt16: {
        auto index_ptr =
            reinterpret_cast<const uint16_t*>(index_buffer.GetData()) + index;
        return *index_ptr;
      }
    }
    return 0u;
  }

  template <class T>
  T GetVertexData(size_t index, size_t offset) const {
    const auto* vtx_ptr = vertex_buffer.GetData() + offset;
    vtx_ptr += GetVertexIndex(index) * pipeline.vertex_descriptor.stride;
    T result;
    ::memmove(&result, vtx_ptr, sizeof(T));
    return result;
  }
};

}  // namespace sft
