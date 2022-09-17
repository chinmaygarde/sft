#pragma once

#include "geom.h"

namespace sft {

class Texture {
 public:
  Texture(const char* path);

  ~Texture();

  bool IsValid() const { return is_valid_; }

  glm::vec4 Sample(glm::vec2 position) const;

 private:
  uint8_t* decoded_ = nullptr;
  glm::ivec2 size_;
  bool is_valid_;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
};

}  // namespace sft
