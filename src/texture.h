#pragma once

#include "geom.h"
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

  glm::vec4 SampleClamped(glm::vec2 position) const;

 private:
  uint8_t* decoded_ = nullptr;
  glm::ivec2 size_;
  Sampler sampler_;
  bool is_valid_;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
};

}  // namespace sft
