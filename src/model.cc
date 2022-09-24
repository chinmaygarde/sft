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

  bool has_normals = false;
  std::vector<ModelShader::VertexData> vertices;
  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      // Triangulation is on.
      size_t fv = 3;

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // Access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

        // Vertex position
        glm::vec3 position(attrib.vertices[3 * idx.vertex_index + 0],
                           attrib.vertices[3 * idx.vertex_index + 1],
                           attrib.vertices[3 * idx.vertex_index + 2]);

        // Vertex normal
        glm::vec3 normal;
        if (idx.normal_index >= 0) {
          has_normals |= true;
          normal.x = attrib.normals[3 * idx.normal_index + 0];
          normal.y = attrib.normals[3 * idx.normal_index + 1];
          normal.z = attrib.normals[3 * idx.normal_index + 2];
        }

        vertices.push_back(
            ModelShader::VertexData{.vertex_color = kColorFirebrick,
                                    .position = position,
                                    .normal = normal});
      }
      index_offset += fv;
    }
  }

  if (!has_normals) {
    SFT_ASSERT(vertices.size() % 3 == 0);
    for (size_t i = 0; i < vertices.size(); i += 3) {
      auto& va = vertices[i + 0];
      auto& vb = vertices[i + 1];
      auto& vc = vertices[i + 2];
      const auto a = va.position;
      const auto b = vb.position;
      const auto c = vc.position;
      auto normal = glm::normalize(glm::cross(c - a, b - a));
      va.normal = vb.normal = vc.normal = normal;
    }
  }

  vertex_count_ = vertices.size();
  vertex_buffer_.Emplace(std::move(vertices));

  pipeline_ = std::make_shared<Pipeline>();
  pipeline_->cull_face = CullFace::kBack;
  pipeline_->winding = Winding::kCounterClockwise;
  pipeline_->shader = std::make_shared<ModelShader>();
  pipeline_->vertex_descriptor.offset =
      offsetof(ModelShader::VertexData, position);
  pipeline_->vertex_descriptor.stride = sizeof(ModelShader::VertexData);

  is_valid_ = true;
}

Model::~Model() = default;

bool Model::IsValid() const {
  return is_valid_;
}

void Model::RenderTo(Rasterizer& rasterizer) {
  if (!IsValid()) {
    return;
  }

  glm::vec2 size = rasterizer.GetSize();

  glm::mat4 proj = glm::ortho(0.0f, size.x, 0.0f, size.y, -60.0f, 60.0f);
  glm::mat4 view = glm::identity<glm::mat4>();
  glm::mat4 model =
      glm::scale(glm::identity<glm::mat4>(), glm::vec3(3.0, 3.0, 1.0));

  const auto mvp = proj * view * model;

  Buffer uniform_buffer;
  uniform_buffer.Emplace(ModelShader::Uniforms{
      .mvp = mvp,
      .light = {0.0, 0.0, -1.0},
  });

  rasterizer.Draw(*pipeline_, vertex_buffer_, uniform_buffer, vertex_count_);
}

}  // namespace sft
