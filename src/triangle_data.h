#pragma once

#include <cstring>
#include <type_traits>

#include "buffer_view.h"
#include "geom.h"
#include "macros.h"
#include "pipeline.h"

namespace sft {

class Buffer;

struct BufferView;

struct Bindings {
  BufferView vertex;
  BufferView index;
  BufferView uniform;
};

struct VertexData {
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  size_t base_vertex_id = 0;
  std::shared_ptr<Pipeline> pipeline;
  Bindings bindings;
  const size_t varyings_stride;
  std::vector<uint8_t> varyings;
  const uint32_t stencil_reference;

  VertexData(std::shared_ptr<Pipeline> p_pipeline,
             Bindings p_bindings,
             size_t p_varyings_stride,
             uint32_t p_stencil_reference)
      : pipeline(std::move(p_pipeline)),
        bindings(std::move(p_bindings)),
        varyings_stride(p_varyings_stride),
        stencil_reference(p_stencil_reference) {
    varyings.resize(varyings_stride * 3u);
  }

  size_t GetVertexIndex(size_t index) const {
    if (!bindings.index) {
      return index;
    }
    switch (pipeline->vertex_descriptor.index_type) {
      case IndexType::kUInt32: {
        auto index_ptr =
            reinterpret_cast<const uint32_t*>(bindings.index.GetData()) + index;
        return *index_ptr;
      }
      case IndexType::kUInt16: {
        auto index_ptr =
            reinterpret_cast<const uint16_t*>(bindings.index.GetData()) + index;
        return *index_ptr;
      }
    }
    return 0u;
  }

  const uint8_t* GetVertexDataPtr(size_t index, size_t offset) const {
    const auto* vtx_ptr = bindings.vertex.GetData() + offset;
    vtx_ptr += GetVertexIndex(index) * pipeline->vertex_descriptor.stride;
    return vtx_ptr;
  }

  template <class T>
  T GetVertexData(size_t index, size_t offset) const {
    T result;
    std::memmove(&result, GetVertexDataPtr(index, offset), sizeof(T));
    return result;
  }

  glm::vec3 GetVertex(size_t index, size_t offset) {
    switch (pipeline->vertex_descriptor.vertex_format) {
      case VertexFormat::kFloat2:
        return {GetVertexData<glm::vec2>(index, offset), 0.0};
      case VertexFormat::kFloat3:
        return GetVertexData<glm::vec3>(index, offset);
    }
  }
};

}  // namespace sft
