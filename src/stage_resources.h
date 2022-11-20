#pragma once

#include <array>
#include <cstring>
#include <type_traits>
#include <vector>

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

  template <class T>
  T LoadUniform(size_t offset) const {
    T result = {};
    memcpy(&result, uniform.buffer.GetData() + offset, sizeof(T));
    return result;
  }
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

  size_t LoadVertexIndex(size_t index) const {
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

  const uint8_t* LoadVertexDataPtr(size_t index, size_t offset) const {
    const auto* vtx_ptr = resources->vertex.GetData() + offset;
    vtx_ptr += LoadVertexIndex(index) * pipeline->vertex_descriptor.stride;
    return vtx_ptr;
  }

  template <class T>
  T LoadVertexData(size_t index, size_t offset) const {
    T result;
    std::memmove(&result, LoadVertexDataPtr(index, offset), sizeof(T));
    return result;
  }

  glm::vec3 LoadVertex(size_t index, size_t offset) {
    switch (pipeline->vertex_descriptor.vertex_format) {
      case VertexFormat::kFloat2:
        return {LoadVertexData<glm::vec2>(index, offset), 0.0};
      case VertexFormat::kFloat3:
        return LoadVertexData<glm::vec3>(index, offset);
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

  template <class T>
  void StoreVarying(const T& val,
                    size_t triangle_index,
                    size_t struct_offset) const {
    auto varyings_offset =
        struct_offset + GetVaryingsStride() * (triangle_index % 3);
    auto ptr = const_cast<uint8_t*>(varyings.data()) + varyings_offset;
    memcpy(ptr, &val, sizeof(T));
  }

  template <class T>
  T LoadVarying(const glm::vec3& barycentric_coordinates,
                size_t struct_offset) const {
    const auto stride = GetVaryingsStride();
    auto ptr = varyings.data() + struct_offset;
    T p1, p2, p3;
    memcpy(&p1, ptr, sizeof(p1));
    ptr += stride;
    memcpy(&p2, ptr, sizeof(p2));
    ptr += stride;
    memcpy(&p3, ptr, sizeof(p3));
    return BarycentricInterpolation(p1, p2, p3, barycentric_coordinates);
  }
};

}  // namespace sft
