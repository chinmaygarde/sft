#pragma once

#include <tiny_obj_loader.h>
#include <iostream>
#include <string>
#include "image.h"

namespace sft {

class Model {
 public:
  Model(std::string path) {
    std::string warnings;
    std::string errors;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    auto result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings,
                                   &errors, path.c_str());

    if (!warnings.empty()) {
      std::cout << warnings << std::endl;
    }

    if (!errors.empty()) {
      std::cout << errors << std::endl;
    }

    if (!result) {
      return;
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        // Triangulation is on.
        int fv = 3;

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
          // access to vertex
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

          // vertex position
          tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
          tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
          tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

          vertices_.push_back(Vec3F{vx, vy, vz});
        }
        index_offset += fv;
      }
    }

    is_valid_ = true;
  }

  bool IsValid() const { return is_valid_; }

  void RenderTo(Image& image) {
    if (!IsValid()) {
      return;
    }

    for (size_t i = 0, count = vertices_.size(); i < count; i += 3) {
      if (i + 2 >= count) {
        return;
      }
      image.DrawTriangle(vertices_[i + 0],  //
                         vertices_[i + 1],  //
                         vertices_[i + 2],  //
                         Color::Random()    //
      );
    }
  }

 private:
  std::vector<Vec3F> vertices_;
  bool is_valid_ = false;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
