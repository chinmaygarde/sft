#pragma once

#include <array>
#include <cstring>
#include <type_traits>

#include "buffer_view.h"
#include "geom.h"
#include "macros.h"
#include "pipeline.h"
#include "uniforms.h"

namespace sft {

class Buffer;

struct BufferView;

struct DispatchResources {
  BufferView vertex;
  BufferView index;
  Uniforms uniform;
};

struct VertexResources {
  std::array<glm::vec3, 3> vtx;
  size_t base_vertex_id = 0;
  std::shared_ptr<Pipeline> pipeline;
  std::shared_ptr<DispatchResources> resources;
  const uint32_t stencil_reference;

  VertexResources(std::shared_ptr<Pipeline> p_pipeline,
                  std::shared_ptr<DispatchResources> p_resources,
                  uint32_t p_stencil_reference)
      : pipeline(std::move(p_pipeline)),
        resources(std::move(p_resources)),
        stencil_reference(p_stencil_reference) {}

  size_t GetVertexIndex(size_t index) const {
    if (!resources->index) {
      return index;
    }
    switch (pipeline->vertex_descriptor.index_type) {
      case IndexType::kUInt32: {
        auto index_ptr =
            reinterpret_cast<const uint32_t*>(resources->index.GetData()) +
            index;
        return *index_ptr;
      }
      case IndexType::kUInt16: {
        auto index_ptr =
            reinterpret_cast<const uint16_t*>(resources->index.GetData()) +
            index;
        return *index_ptr;
      }
    }
    return 0u;
  }

  const uint8_t* GetVertexDataPtr(size_t index, size_t offset) const {
    const auto* vtx_ptr = resources->vertex.GetData() + offset;
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

struct FragmentResources {
  Rect box;
  glm::vec3 ndc[3];
  std::shared_ptr<Pipeline> pipeline;
  std::shared_ptr<DispatchResources> resources;
  uint32_t stencil_reference = 0;
  std::vector<uint8_t> varyings;

  explicit FragmentResources(size_t varyings_stride) {
    varyings.resize(varyings_stride * 3u);
  }

  size_t GetVaryingsStride() const { return varyings.size() / 3u; }

  const Image& LoadImage(size_t location) const {
    return *resources->uniform.images.at(location);
  }
};

}  // namespace sft
