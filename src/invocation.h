#pragma once

#include "geom.h"
#include "macros.h"
#include "rasterizer.h"
#include "tiler.h"

namespace sft {

class Rasterizer;
struct TriangleData;

struct VertexInvocation {
  size_t vertex_id;

  template <class T>
  T LoadVertexData(size_t offset) const {
    return rasterizer.LoadVertexData<T>(data, vertex_id, offset);
  }

  template <class T>
  T LoadUniform(size_t offset) const {
    return rasterizer.LoadUniform<T>(data, offset);
  }

  template <class T>
  void StoreVarying(const T& val, size_t offset) const {
    rasterizer.StoreVarying(data, val, vertex_id, offset);
  }

 private:
  friend Rasterizer;

  const Rasterizer& rasterizer;
  const TriangleData& data;
  const Tiler::Data& tiler_data;

  VertexInvocation(const Rasterizer& p_rasterizer,
                   const TriangleData& p_data,
                   const Tiler::Data& p_tiler_data,
                   size_t p_vertex_id)
      : vertex_id(p_vertex_id),
        rasterizer(p_rasterizer),
        data(p_data),
        tiler_data(p_tiler_data) {}
};

struct FragmentInvocation {
  glm::vec3 barycentric_coordinates;

  template <class T>
  T LoadVarying(size_t offset) const {
    return rasterizer.LoadVarying<T>(data,                     //
                                     barycentric_coordinates,  //
                                     offset                    //
    );
  }

  template <class T>
  T LoadUniform(size_t offset) const {
    return rasterizer.LoadUniform<T>(data, offset);
  }

 private:
  friend Rasterizer;

  const Rasterizer& rasterizer;
  const TriangleData& data;
  const Tiler::Data& tiler_data;

  FragmentInvocation(glm::vec3 p_barycentric_coordinates,
                     const Rasterizer& p_rasterizer,
                     const TriangleData& p_data,
                     const Tiler::Data& p_tiler_data)
      : barycentric_coordinates(p_barycentric_coordinates),
        rasterizer(p_rasterizer),
        data(p_data),
        tiler_data(p_tiler_data) {}
};

}  // namespace sft
