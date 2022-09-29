#pragma once

#include "geom.h"
#include "macros.h"
#include "sampler.h"

namespace sft {

class Texture {
 public:
  Texture(const char* path);

  ~Texture();

  void SetSampler(Sampler sampler) { sampler_ = std::move(sampler); }

  const Sampler& GetSampler() const { return sampler_; }

  bool IsValid() const { return is_valid_; }

  glm::vec4 Sample(glm::vec2 position) const;

 private:
  uint8_t* decoded_ = nullptr;
  glm::ivec2 size_;
  Sampler sampler_;
  bool is_valid_;

  glm::vec4 SampleUnit(glm::vec2 position) const;

  glm::vec4 SampleUnitNearest(glm::vec2 pos) const;

  glm::vec4 SampleUnitLinear(glm::vec2 pos) const;

  glm::vec4 SampleUV(glm::ivec2 uv) const;

  SFT_DISALLOW_COPY_AND_ASSIGN(Texture);
};

}  // namespace sft
