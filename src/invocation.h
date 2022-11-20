#pragma once

#include "geom.h"
#include "macros.h"
#include "rasterizer.h"
#include "tiler.h"

namespace sft {

class Rasterizer;
struct VertexResources;

struct VertexInvocation {
  size_t vtx_index;

  template <class T>
  T LoadVertexData(size_t offset) const {
    return vtx_resources.LoadVertexData<T>(vtx_index, offset);
  }

  template <class T>
  T LoadUniform(size_t struct_offset) const {
    return vtx_resources.resources->LoadUniform<T>(struct_offset);
  }

  template <class T>
  void StoreVarying(const T& value, size_t struct_offset) const {
    frag_resources.StoreVarying(value, vtx_index, struct_offset);
  }

 private:
  friend Rasterizer;

  const VertexResources& vtx_resources;
  const FragmentResources& frag_resources;

  VertexInvocation(const VertexResources& p_vtx_resources,
                   const FragmentResources& p_frag_resources,
                   size_t p_vertex_id)
      : vtx_index(p_vertex_id),
        vtx_resources(p_vtx_resources),
        frag_resources(p_frag_resources) {}
};

struct FragmentInvocation {
  glm::vec3 barycentric_coordinates;

  template <class T>
  T LoadVarying(size_t offset) const {
    return frag_resources.LoadVarying<T>(barycentric_coordinates,  //
                                         offset                    //
    );
  }

  template <class T>
  T LoadUniform(size_t struct_offset) const {
    return frag_resources.resources->LoadUniform<T>(struct_offset);
  }

  const Image& LoadImage(size_t location) const {
    return frag_resources.LoadImage(location);
  }

 private:
  friend Rasterizer;

  const FragmentResources& frag_resources;

  FragmentInvocation(glm::vec3 p_barycentric_coordinates,
                     const FragmentResources& p_resources)
      : barycentric_coordinates(p_barycentric_coordinates),
        frag_resources(p_resources) {}
};

}  // namespace sft
