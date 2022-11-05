#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <type_traits>

#include "geom.h"
#include "image.h"
#include "macros.h"
#include "marl/scheduler.h"
#include "marl/waitgroup.h"

namespace sft {

enum class SampleCount : uint8_t {
  kOne = 1,
  kTwo = 2,
  kFour = 4,
  kEight = 8,
  kSixteen = 16,
};

constexpr size_t GetSampleCount(SampleCount count) {
  return static_cast<uint8_t>(count);
}

constexpr std::array<glm::vec2, 1u> kSampleLocationsOne = {
    glm::vec2{0.5f, 0.5f}};

constexpr std::array<glm::vec2, 2u> kSampleLocationsTwo = {
    glm::vec2{0.75f, 0.75f},
    glm::vec2{0.25f, 0.25f},
};

constexpr std::array<glm::vec2, 4u> kSampleLocationsFour = {
    glm::vec2{0.375f, 0.125f},
    glm::vec2{0.875f, 0.375f},
    glm::vec2{0.125f, 0.625f},
    glm::vec2{0.625f, 0.875f},
};

constexpr std::array<glm::vec2, 8u> kSampleLocationsEight = {
    glm::vec2{0.5625, 0.3125}, glm::vec2{0.4375, 0.6875},
    glm::vec2{0.8125, 0.5625}, glm::vec2{0.3125, 0.1875},
    glm::vec2{0.1875, 0.8125}, glm::vec2{0.0625, 0.4375},
    glm::vec2{0.6875, 0.9375}, glm::vec2{0.9375, 0.0625},
};

constexpr std::array<glm::vec2, 16u> kSampleLocationsSixteen = {
    glm::vec2{0.5625f, 0.5625f}, glm::vec2{0.4375f, 0.3125f},
    glm::vec2{0.3125f, 0.625f},  glm::vec2{0.75f, 0.4375f},
    glm::vec2{0.1875f, 0.375f},  glm::vec2{0.625f, 0.8125f},
    glm::vec2{0.8125f, 0.6875f}, glm::vec2{0.6875f, 0.1875f},
    glm::vec2{0.375f, 0.875f},   glm::vec2{0.5f, 0.0625f},
    glm::vec2{0.25f, 0.125f},    glm::vec2{0.125f, 0.75f},
    glm::vec2{0.0f, 0.5f},       glm::vec2{0.9375f, 0.25f},
    glm::vec2{0.875f, 0.9375f},  glm::vec2{0.0625f, 0.0f},
};

// From: Multisampling: Standard sample locations
// https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#primsrast-multisampling
constexpr glm::vec2 GetSampleLocation(SampleCount sample_count,
                                      size_t location) {
  switch (sample_count) {
    case SampleCount::kOne:
      return kSampleLocationsOne[location % 1];
    case SampleCount::kTwo:
      return kSampleLocationsTwo[location % 2];
    case SampleCount::kFour:
      return kSampleLocationsFour[location % 4];
    case SampleCount::kEight:
      return kSampleLocationsEight[location % 8];
    case SampleCount::kSixteen:
      return kSampleLocationsSixteen[location % 16];
  }
  return {0.5, 0.5};
}

inline Color PerformResolve2(Color a, Color b) {
  return Color{(a.color & b.color) + (((a.color ^ b.color) >> 1) & 0x7F7F7F7F) +
               ((a.color ^ b.color) & 0x01010101)};
}

inline Color PerformResolve(const Color* samples, uint8_t count) {
  if (count == 1) {
    return samples[0];
  }
  auto* intermediates =
      reinterpret_cast<Color*>(alloca(sizeof(Color) * count / 2));
  for (size_t i = 0; i < count; i += 2) {
    intermediates[i / 2] = PerformResolve2(samples[i], samples[i + 1]);
  }
  return PerformResolve(intermediates, count / 2);
}

template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
class Texture {
 public:
  Texture(glm::ivec2 size, SampleCount samples = SampleCount::kOne)
      : Texture(reinterpret_cast<T*>(
                    std::calloc(size.x * size.y * static_cast<uint8_t>(samples),
                                sizeof(T))),
                size,
                samples) {}

  ~Texture() { std::free(allocation_); }

  bool IsValid() const { return allocation_ != nullptr; }

  [[nodiscard]] bool Resize(glm::ivec2 size) {
    TRACE_EVENT(kTraceCategoryRasterizer, "RenderPass::Resize");
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

  [[nodiscard]] bool UpdateSampleCount(SampleCount sample_count) {
    if (sample_count_ == sample_count) {
      return true;
    }

    sample_count_ = sample_count;
    return Resize(size_);
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
    TRACE_EVENT(kTraceCategoryRasterizer, "Texture<T>::Clear");
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

  std::shared_ptr<Image> CreateImage(
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
    return std::make_shared<Image>(mapping, size_);
  }

  glm::ivec2 GetSize() const { return size_; }

  SampleCount GetSampleCount() const { return sample_count_; }

  [[nodiscard]] bool Resolve(Texture<T>& to) const {
    if (to.GetSize() != GetSize()) {
      return false;
    }
    if (to.GetSampleCount() != SampleCount::kOne) {
      return false;
    }
    constexpr size_t slices = 5u;
    glm::ivec2 span = size_ / glm::ivec2{slices, slices};

    marl::WaitGroup wg;

    for (size_t x = 0; x < slices; x++) {
      for (size_t y = 0; y < slices; y++) {
        wg.add();
        const auto min = glm::ivec2{span.x * x, span.y * y};
        const auto max = glm::ivec2{span.x * (x + 1), span.y * (y + 1)};
        marl::schedule([&, min, max]() {
          ResolveSubSection(to, min, max);
          wg.done();
        });
      }
    }
    wg.wait();
    return true;
  }

 private:
  T* allocation_ = nullptr;
  glm::ivec2 size_ = {};
  SampleCount sample_count_;

  Texture(T* allocation, glm::ivec2 size, SampleCount sample_count)
      : allocation_(allocation), size_(size), sample_count_(sample_count) {}

  void ResolveSubSection(Texture<T>& to, glm::ivec2 min, glm::ivec2 max) const {
    min = glm::max(glm::ivec2{0, 0}, min);
    max = glm::min(size_, max);
    for (auto x = min.x; x < max.x; x++) {
      for (auto y = min.y; y < max.y; y++) {
        const auto position = glm::ivec2{x, y};
        const auto* samples = Get(position, 0);
        to.Set(PerformResolve(samples, static_cast<uint8_t>(sample_count_)),
               position, 0);
      }
    }
  }

  SFT_DISALLOW_COPY_AND_ASSIGN(Texture);
};

}  // namespace sft
