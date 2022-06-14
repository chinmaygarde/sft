#pragma once

#include <tiny_obj_loader.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include "geom.h"
#include "rasterizer.h"

namespace sft {

class Model {
 public:
  Model(std::string path);

  ~Model();

  bool IsValid() const;

  void RenderTo(Rasterizer& image);

  void SetTransformation(glm::mat4 xformation);

 private:
  std::vector<glm::vec4> vertices_;
  glm::mat4 xformation_ = glm::identity<glm::mat4>();
  bool is_valid_ = false;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
