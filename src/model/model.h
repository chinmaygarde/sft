/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <tiny_obj_loader.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "buffer.h"
#include "geometry.h"
#include "image.h"
#include "model_shader.h"
#include "rasterizer.h"

namespace sft {

class Model {
 public:
  Model(std::string path, std::string base_dir);

  ~Model();

  bool IsValid() const;

  void RenderTo(Rasterizer& rasterizer);

  void SetScale(ScalarF scale);

  void SetRotation(ScalarF degrees);

  void SetTexture(std::shared_ptr<Image> texture);

  void SetLightDirection(glm::vec3 dir);

  Pipeline& GetPipeline();

 private:
  std::shared_ptr<ModelShader> model_shader_;
  std::shared_ptr<Pipeline> pipeline_;
  std::shared_ptr<Buffer> vertex_buffer_;
  std::shared_ptr<Image> texture_;
  size_t vertex_count_ = 0u;
  ScalarF scale_ = 1.0f;
  ScalarF rotation_ = 0.0f;
  glm::vec3 light_direction_ = {0.0, 0.0, 1.0};
  bool is_valid_ = false;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
