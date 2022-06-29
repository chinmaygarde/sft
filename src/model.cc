#include "model.h"

namespace sft {

Model::Model(std::string path) {
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
      size_t fv = 3;

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

        // vertex position
        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
        tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
        tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

        vertices_.push_back({vx, vy, vz, 1.0});
      }
      index_offset += fv;
    }
  }

  is_valid_ = true;
}

Model::~Model() = default;

bool Model::IsValid() const {
  return is_valid_;
}

void Model::SetTransformation(glm::mat4 xformation) {
  xformation_ = std::move(xformation);
}

void Model::RenderTo(Rasterizer& image) {
  if (!IsValid()) {
    return;
  }

  glm::vec2 size = image.GetSize();

  glm::mat4 proj = glm::orthoLH_NO(0.0f, size.x, 0.0f, size.y, -50.0f, 50.0f);
  glm::mat4 view =
      glm::translate(glm::identity<glm::mat4>(),
                     glm::vec3(size.x / 2.0, -75.0 + (size.y / 2.0), 0.0)) *
      glm::scale(glm::identity<glm::mat4>(), glm::vec3(4.0, 4.0, 1.0));
  glm::mat4 model = xformation_;

  const auto mvp = proj * view * model;
  for (size_t i = 0, count = vertices_.size(); i < count; i += 3) {
    if (i + 2 >= count) {
      return;
    }
    glm::vec3 p1 = mvp * vertices_[i + 0];
    glm::vec3 p2 = mvp * vertices_[i + 1];
    glm::vec3 p3 = mvp * vertices_[i + 2];

    glm::vec3 light_direction = {0, 0, -1};
    glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
    float intensity = glm::dot(light_direction, normal);
    if (intensity >= 0.0) {
      image.DrawTriangle(p1, p2, p3);
    }
  }
}

}  // namespace sft
