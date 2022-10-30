#pragma once

#include "geom.h"
#include "macros.h"
#include "rasterizer.h"
#include "tiler.h"

namespace sft {

class Rasterizer;
struct VertexData;

struct VertexInvocation {
  size_t vtx_index;

  template <class T>
  T LoadVertexData(size_t offset) const {
    return rasterizer.LoadVertexData<T>(vtx_data, vtx_index, offset);
  }

  template <class T>
  T LoadUniform(size_t offset) const {
    return rasterizer.LoadUniform<T>(*vtx_data.resources, offset);
  }

  template <class T>
  void StoreVarying(const T& val, size_t offset) const {
    rasterizer.StoreVarying(*vtx_data.resources, val, vtx_index, offset);
  }

 private:
  friend Rasterizer;

  const Rasterizer& rasterizer;
  const VertexData& vtx_data;
  const Tiler::Data& tiler_data;

  VertexInvocation(const Rasterizer& p_rasterizer,
                   const VertexData& p_vtx_data,
                   const Tiler::Data& p_tiler_data,
                   size_t p_vertex_id)
      : vtx_index(p_vertex_id),
        rasterizer(p_rasterizer),
        vtx_data(p_vtx_data),
        tiler_data(p_tiler_data) {}
};

struct FragmentInvocation {
  glm::vec3 barycentric_coordinates;

  template <class T>
  T LoadVarying(size_t offset) const {
    return rasterizer.LoadVarying<T>(resources,                //
                                     barycentric_coordinates,  //
                                     offset                    //
    );
  }

  template <class T>
  T LoadUniform(size_t offset) const {
    return rasterizer.LoadUniform<T>(resources, offset);
  }

 private:
  friend Rasterizer;

  const Rasterizer& rasterizer;
  const Resources& resources;

  FragmentInvocation(glm::vec3 p_barycentric_coordinates,
                     const Rasterizer& p_rasterizer,
                     const Resources& p_resources)
      : barycentric_coordinates(p_barycentric_coordinates),
        rasterizer(p_rasterizer),
        resources(p_resources) {}
};

}  // namespace sft
