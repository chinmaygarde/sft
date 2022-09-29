#include "sampler.h"

#include "texture.h"

namespace sft {

constexpr ScalarF SamplerLocation(ScalarF location, WrapMode mode) {
  // Section 3.7.6 "Texture Wrap Modes"
  // https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf
  switch (mode) {
    case WrapMode::kClamp:
      return glm::clamp(location, 0.0f, 1.0f);
    case WrapMode::kRepeat:
      return glm::fract(location);
    case WrapMode::kMirror: {
      const auto is_even = static_cast<int>(glm::floor(location)) % 2 == 0;
      if (is_even) {
        return glm::fract(location);
      } else {
        return 1.0f - glm::fract(location);
      }
    }
  }
  return 0.0;
}

glm::vec4 Sampler::Sample(glm::vec2 location, const Texture& texture) const {
  return texture.SampleClamped({SamplerLocation(location.x, wrap_mode_s),
                                SamplerLocation(location.y, wrap_mode_t)});
}

}  // namespace sft
