#pragma once

#include "geom.h"

namespace sft {

class Shader {
 public:
  virtual ~Shader() = default;

  virtual glm::vec3 ProcessVertex(glm::vec3 in) = 0;

  virtual Color ProcessFragment(glm::vec3 bary_pos) = 0;
};

}  // namespace sft
