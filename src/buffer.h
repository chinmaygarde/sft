#pragma once

#include <memory>
#include <vector>

#include "macros.h"

namespace sft {

class Buffer final : public std::enable_shared_from_this<Buffer> {
 public:
  static std::shared_ptr<Buffer> Create() {
    return std::shared_ptr<Buffer>(new Buffer());
  }

  ~Buffer() = default;

  template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
  void Emplace(const T& object) {
    Emplace(reinterpret_cast<const uint8_t*>(&object), sizeof(T));
  }

  template <class T>
  void Emplace(const std::vector<T>& items) {
    for (const auto& item : items) {
      Emplace(item);
    }
  }

  void Emplace(const uint8_t* buffer, size_t size) {
    const auto old_size = buffer_.size();
    buffer_.resize(old_size + size);
    memmove(buffer_.data() + old_size, buffer, size);
  }

  const uint8_t* GetData() const { return buffer_.data(); }

  std::shared_ptr<Buffer> AsShared() { return shared_from_this(); }

 private:
  std::vector<uint8_t> buffer_;

  Buffer() = default;

  SFT_DISALLOW_COPY_AND_ASSIGN(Buffer);
};

}  // namespace sft
