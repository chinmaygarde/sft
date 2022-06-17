#pragma once

#include <optional>
#include "geom.h"

namespace sft {

class Shader {
 public:
  virtual ~Shader() = default;

  virtual glm::vec3 ProcessVertex(glm::vec3 in) = 0;

  virtual std::optional<Color> ProcessFragment(glm::vec3 bary_pos) = 0;
};

}  // namespace sft
