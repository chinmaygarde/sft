#pragma once

#include <tiny_obj_loader.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include "geom.h"
#include "image.h"

namespace sft {

class Model {
 public:
  Model(std::string path);

  ~Model();

  bool IsValid() const;

  void RenderTo(Image& image);

 private:
  std::vector<glm::vec4> vertices_;
  bool is_valid_ = false;
  const std::chrono::high_resolution_clock::time_point start_time_ =
      std::chrono::high_resolution_clock::now();

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
