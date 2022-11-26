/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "model.h"

namespace sft {

Model::Model(std::string path, std::string base_dir)
    : vertex_buffer_(Buffer::Create()) {
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
  vertex_buffer_->Emplace(std::move(vertices));

  pipeline_ = std::make_shared<Pipeline>();
  pipeline_->depth_desc.depth_test_enabled = true;
  pipeline_->cull_face = CullFace::kBack;
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

  if (!texture_) {
    return;
  }

  glm::vec2 size = rasterizer.GetSize();

  auto proj =
      glm::perspectiveLH_ZO(glm::radians(90.f), size.x / size.y, 0.1f, 1000.0f);
  auto view = glm::lookAtLH(glm::vec3{0, 5, -10},  // eye
                            glm::vec3{0, 0, 0},    // center
                            glm::vec3{0, 1, 0}     // up
  );
  auto scale = glm::scale(glm::mat4{1.0f}, glm::vec3{scale_, scale_, scale_});
  auto rotation =
      glm::rotate(glm::mat4{1.0f}, glm::radians(rotation_), {0, 1, 0});
  auto model = scale * rotation;
  const auto mvp = proj * view * model;

  auto uniform_buffer = Buffer::Create();
  uniform_buffer->Emplace(ModelShader::Uniforms{
      .mvp = mvp,
      .light = light_direction_,
      .color = kColorFirebrick,
  });
  sft::Uniforms uniforms;
  uniforms.buffer = *uniform_buffer;
  uniforms.images[0] = texture_;
  rasterizer.Draw(pipeline_, *vertex_buffer_, uniforms, vertex_count_);
}

void Model::SetScale(ScalarF scale) {
  scale_ = scale;
}

void Model::SetRotation(ScalarF rotation) {
  rotation_ = rotation;
}

void Model::SetTexture(std::shared_ptr<Image> texture) {
  texture_ = std::move(texture);
}

void Model::SetLightDirection(glm::vec3 dir) {
  light_direction_ = glm::normalize(dir);
}

Pipeline& Model::GetPipeline() {
  return *pipeline_;
}

}  // namespace sft
