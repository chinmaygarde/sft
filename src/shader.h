#pragma once

#include <optional>
#include "geom.h"

namespace sft {

class Shader {
 public:
  virtual ~Shader() = default;

  virtual glm::vec3 ProcessVertex(glm::vec3 in, size_t index) = 0;

  virtual std::optional<Color> ProcessFragment(glm::vec3 bary_pos,
                                               size_t index) = 0;
};

}  // namespace sft
