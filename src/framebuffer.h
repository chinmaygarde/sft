#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <type_traits>

#include "geom.h"
#include "macros.h"
#include "texture.h"

namespace sft {

enum class SampleCount : uint8_t {
  kOne = 1,
  kFour = 4,
};

constexpr size_t GetSampleCount(SampleCount count) {
  return static_cast<uint8_t>(count);
}

constexpr std::array<glm::vec2, 1u> kSampleLocationsOne = {
    glm::vec2{0.5f, 0.5f}};

constexpr std::array<glm::vec2, 4u> kSampleLocationsFour = {
    glm::vec2{0.375f, 0.125f},
    glm::vec2{0.875f, 0.375f},
    glm::vec2{0.125f, 0.625f},
    glm::vec2{0.625f, 0.875f},
};

// From: Multisampling: Standard sample locations
// https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#primsrast-multisampling
constexpr glm::vec2 GetSampleLocation(SampleCount sample_count,
                                      size_t location) {
  switch (sample_count) {
    case SampleCount::kOne:
      return kSampleLocationsOne[0];
    case SampleCount::kFour:
      return kSampleLocationsFour[location % 4];
  }
  return {0.5, 0.5};
}

template <class Type>
Type Resolve(const Type* samples, SampleCount sample_count);

template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
class Framebuffer {
 public:
  Framebuffer(glm::ivec2 size, SampleCount samples = SampleCount::kOne)
      : Framebuffer(reinterpret_cast<T*>(std::calloc(
                        size.x * size.y * static_cast<uint8_t>(samples),
                        sizeof(T))),
                    size,
                    samples) {}

  ~Framebuffer() { std::free(allocation_); }

  bool IsValid() const { return allocation_ != nullptr; }

  [[nodiscard]] bool Resize(glm::ivec2 size) {
    auto new_allocation =
        std::realloc(allocation_, size.x * size.y * sizeof(T) *
                                      static_cast<uint8_t>(sample_count_));
    if (!new_allocation) {
      // The old allocation is still valid. Nothing has changed.
      return false;
    }
    allocation_ = reinterpret_cast<T*>(new_allocation);
    size_ = size;
    return true;
  }

  void Set(const T& val, glm::ivec2 pos, size_t sample_index) {
    const auto sample_count = static_cast<uint8_t>(sample_count_);
    const auto offset = ((size_.x * pos.y + pos.x) * sample_count) +
                        (sample_index % sample_count);
    std::memcpy(allocation_ + offset, &val, sizeof(T));
  }

  const T* Get(glm::ivec2 pos, size_t sample_index) const {
    const auto sample_count = static_cast<uint8_t>(sample_count_);
    const auto offset = ((size_.x * pos.y + pos.x) * sample_count) +
                        (sample_index % sample_count);
    return allocation_ + offset;
  }

  void Clear(const T& val) {
    for (auto i = 0;
         i < size_.x * size_.y * static_cast<uint8_t>(sample_count_); i++) {
      allocation_[i] = val;
    }
  }

  constexpr size_t GetBytesPerPixel() const { return sizeof(T); }

  constexpr size_t GetByteLength() const {
    return GetLength() * GetBytesPerPixel();
  }

  constexpr size_t GetLength() const {
    return size_.x * size_.y * static_cast<uint8_t>(sample_count_);
  }

  std::pair<T, T> GetMinMaxValue() const {
    if (sample_count_ != SampleCount::kOne) {
      return {};
    }
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
    if (sample_count_ != SampleCount::kOne) {
      return nullptr;
    }
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

  glm::ivec2 GetSize() const { return size_; }

  SampleCount GetSampleCount() const { return sample_count_; }

  [[nodiscard]] bool Resolve(Framebuffer<T>& to) const {
    if (to.GetSize() != GetSize()) {
      return false;
    }
    if (to.GetSampleCount() != SampleCount::kOne) {
      return false;
    }
    for (auto x = 0; x < size_.x; x++) {
      for (auto y = 0; y < size_.y; y++) {
        const auto position = glm::ivec2{x, y};
        const auto* samples = Get(position, 0);
        to.Set(Resolve(samples, sample_count_));
      }
    }
  }

 private:
  T* allocation_ = nullptr;
  glm::ivec2 size_ = {};
  const SampleCount sample_count_;

  Framebuffer(T* allocation, glm::ivec2 size, SampleCount sample_count)
      : allocation_(allocation), size_(size), sample_count_(sample_count) {}

  SFT_DISALLOW_COPY_AND_ASSIGN(Framebuffer);
};

}  // namespace sft
