#pragma once

#include <memory>

#include "geom.h"
#include "macros.h"
#include "mapping.h"
#include "sampler.h"

namespace sft {

class Image final : public std::enable_shared_from_this<Image> {
 public:
  Image(const char* file_path);

  Image(std::shared_ptr<Mapping> mapping, glm::ivec2 size);

  ~Image();

  void SetSampler(Sampler sampler) { sampler_ = std::move(sampler); }

  const Sampler& GetSampler() const { return sampler_; }

  glm::ivec2 GetSize() const { return size_; }

  bool IsValid() const { return is_valid_; }

  glm::vec4 Sample(glm::vec2 position) const;

 private:
  std::shared_ptr<Mapping> mapping_;
  glm::ivec2 size_;
  Sampler sampler_;
  bool is_valid_;

  glm::vec4 SampleUnit(glm::vec2 position) const;

  glm::vec4 SampleUnitNearest(glm::vec2 pos) const;

  glm::vec4 SampleUnitLinear(glm::vec2 pos) const;

  glm::vec4 SampleUV(glm::ivec2 uv) const;

  const uint8_t* GetBuffer() const;

  SFT_DISALLOW_COPY_AND_ASSIGN(Image);
};

}  // namespace sft
