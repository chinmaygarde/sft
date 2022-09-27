#include "model.h"

namespace sft {

Model::Model(std::string path, std::string base_dir) {
  std::string warnings;
  std::string errors;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  auto result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings,
                                 &errors, path.c_str(), base_dir.c_str());

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
          normal = glm::normalize(normal);
        }

        // Texture coords.
        glm::vec2 texture_coord;
        if (idx.texcoord_index >= 0) {
          texture_coord =
              glm::vec2({attrib.texcoords[2 * idx.texcoord_index + 0],
                         attrib.texcoords[2 * idx.texcoord_index + 1]});
          texture_coord = glm::abs(texture_coord);
        }

        vertices.push_back(
            ModelShader::VertexData{.position = position,
                                    .normal = normal,
                                    .texture_coord = texture_coord});
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
  pipeline_->depth_test_enabled = true;
  pipeline_->cull_face = CullFace::kBack;
  pipeline_->winding = Winding::kCounterClockwise;
  model_shader_ = std::make_shared<ModelShader>();
  pipeline_->shader = model_shader_;
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

  glm::mat4 proj = glm::ortho(0.0f, size.x, 0.0f, size.y, -10000.0f, 10000.0f);
  glm::mat4 view =
      glm::translate(glm::identity<glm::mat4>(), {size.x / 2, size.y / 2, 0.0});
  glm::mat4 scale =
      glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale_, scale_, 1.0));
  glm::mat4 rotate =
      glm::rotate(glm::identity<glm::mat4>(), glm::radians(rotation_),
                  glm::vec3(0.0, 1.0, 0.0));

  const auto mvp = proj * view * scale * rotate;

  Buffer uniform_buffer;
  uniform_buffer.Emplace(ModelShader::Uniforms{
      .mvp = mvp,
      .light = light_direction_,
      .color = kColorFirebrick,
  });

  rasterizer.Draw(*pipeline_, vertex_buffer_, uniform_buffer, vertex_count_);
}

void Model::SetScale(ScalarF scale) {
  scale_ = scale;
}

void Model::SetRotation(ScalarF rotation) {
  rotation_ = rotation;
}

void Model::SetTexture(std::shared_ptr<Texture> texture) {
  model_shader_->SetTexture(std::move(texture));
}

void Model::SetLightDirection(glm::vec3 dir) {
  light_direction_ = glm::normalize(dir);
}

}  // namespace sft
