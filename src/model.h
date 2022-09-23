#pragma once

#include <tiny_obj_loader.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "buffer.h"
#include "geom.h"
#include "rasterizer.h"
#include "shaders/model_shader.h"

namespace sft {

class Model {
 public:
  Model(std::string path);

  ~Model();

  bool IsValid() const;

  void RenderTo(Rasterizer& rasterizer);

 private:
  std::shared_ptr<Pipeline> pipeline_;
  Buffer vertex_buffer_;
  size_t vertex_count_ = 0u;
  bool is_valid_ = false;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
