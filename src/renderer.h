#pragma once

#include "geom.h"

namespace sft {

class Renderer {
 public:
  virtual ~Renderer() {}

  virtual const void* GetPixels() const = 0;

  virtual glm::ivec2 GetSize() const = 0;

  virtual size_t GetBytesPerPixel() const = 0;
};

}  // namespace sft
