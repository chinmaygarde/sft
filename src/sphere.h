#pragma once

#include "geom.h"
#include "macros.h"

namespace sft {

struct Sphere {
  glm::vec3 center;
  ScalarF radius = 0.0;

  bool RayIntersects(glm::vec3 origin, glm::vec3 direction) const {
    return true;
  }
};
}  // namespace sft
