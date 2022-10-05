#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>

#include "geom.h"
#include "macros.h"
#include "texture.h"

namespace sft {

class FramebufferBase {
 public:
  FramebufferBase() = default;

  virtual ~FramebufferBase() = default;

  virtual glm::ivec2 GetSize() const = 0;
};

template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
class Framebuffer final : public FramebufferBase {
 public:
  Framebuffer(glm::ivec2 size)
      : Framebuffer(
            reinterpret_cast<T*>(std::calloc(size.x * size.y, sizeof(T))),
            size) {}

  ~Framebuffer() override { std::free(allocation_); }

  bool IsValid() const { return allocation_ != nullptr; }

  [[nodiscard]] bool Resize(glm::ivec2 size) {
    auto new_allocation =
        std::realloc(allocation_, size.x * size.y * sizeof(T));
    if (!new_allocation) {
      // The old allocation is still valid. Nothing has changed.
      return false;
    }
    allocation_ = reinterpret_cast<T*>(new_allocation);
    size_ = size;
    return true;
  }

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

  constexpr size_t GetByteLength() const {
    return GetLength() * GetBytesPerPixel();
  }

  constexpr size_t GetLength() const { return size_.x * size_.y; }

  std::pair<T, T> GetMinMaxValue() const {
    auto min = std::numeric_limits<T>::max();
    auto max = std::numeric_limits<T>::min();
    for (size_t i = 0, count = GetLength(); i < count; i++) {
      min = std::min(min, allocation_[i]);
      max = std::max(max, allocation_[i]);
    }
    return {min, max};
  }

  std::shared_ptr<Texture> CreateTexture(
      std::function<Color(const T&)> transform) const {
    const auto size = GetLength() * sizeof(Color);
    auto* allocation = reinterpret_cast<Color*>(std::malloc(size));
    if (!allocation) {
      return nullptr;
    }
    for (size_t i = 0; i < GetLength(); i++) {
      allocation[i] = transform(allocation_[i]).BGRA2RGBA();
    }
    auto mapping = std::make_shared<Mapping>(
        reinterpret_cast<const uint8_t*>(allocation),  //
        size,                                          //
        [allocation]() { std::free(allocation); }      //
    );
    return std::make_shared<Texture>(mapping, size_);
  }

  glm::ivec2 GetSize() const override { return size_; }

 private:
  T* allocation_ = nullptr;
  glm::ivec2 size_ = {};

  Framebuffer(T* allocation, glm::ivec2 size)
      : allocation_(allocation), size_(size) {}

  SFT_DISALLOW_COPY_AND_ASSIGN(Framebuffer);
};

}  // namespace sft
