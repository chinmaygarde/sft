#pragma once

#include <type_traits>

#include "geom.h"
#include "macros.h"

namespace sft {

template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
class Framebuffer {
 public:
  Framebuffer(glm::ivec2 size)
      : allocation_(
            reinterpret_cast<T*>(std::calloc(size.x * size.y, sizeof(T)))),
        size_(size) {}

  ~Framebuffer() { std::free(allocation_); }

  bool IsValid() const { return allocation_ != nullptr; }

  void Set(const T& val, glm::ivec2 pos) {
    const auto offset = size_.x * pos.y + pos.x;
    std::memcpy(allocation_ + offset, &val, sizeof(T));
  }

  const T* Get(glm::ivec2 pos = {0, 0}) const {
    const auto offset = size_.x * pos.y + pos.x;
    return allocation_ + offset;
  }

  void Clear(const T& val) {
    for (auto i = 0; i < size_.x * size_.y; i++) {
      allocation_[i] = val;
    }
  }

  constexpr size_t GetBytesPerPixel() const { return sizeof(T); }

 private:
  T* allocation_ = nullptr;
  glm::ivec2 size_ = {};
};

}  // namespace sft
