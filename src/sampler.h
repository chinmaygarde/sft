#pragma once

#include "geom.h"

namespace sft {

enum class WrapMode {
  kRepeat,
  kClamp,
  kMirror,
};

class Texture;

struct Sampler {
  WrapMode wrap_mode_s = WrapMode::kRepeat;
  WrapMode wrap_mode_t = WrapMode::kRepeat;

  glm::vec4 Sample(glm::vec2 location, const Texture& texture) const;
};

}  // namespace sft
