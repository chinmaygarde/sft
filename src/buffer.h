#pragma once

#include <memory>
#include <vector>

#include "buffer_view.h"
#include "macros.h"

namespace sft {

class Buffer final : public std::enable_shared_from_this<Buffer> {
 public:
  static std::shared_ptr<Buffer> Create() {
    return std::shared_ptr<Buffer>(new Buffer());
  }

  ~Buffer() = default;

  template <class T, class = std::enable_if_t<std::is_standard_layout_v<T>>>
  BufferView Emplace(const T& object) {
    return Emplace(reinterpret_cast<const uint8_t*>(&object), sizeof(T));
  }

  template <class T>
  BufferView Emplace(const std::vector<T>& items) {
    const auto old_length = buffer_.size();
    for (const auto& item : items) {
      Emplace(item);
    }
    return BufferView(*this, old_length, GetLength() - old_length);
  }

  BufferView Emplace(const uint8_t* buffer, size_t size) {
    const auto old_length = buffer_.size();
    buffer_.resize(old_length + size);
    memmove(buffer_.data() + old_length, buffer, size);
    return {*this, old_length, GetLength() - old_length};
  }

  const uint8_t* GetData() const;

  size_t GetLength() const;

  std::shared_ptr<Buffer> AsShared();

 private:
  std::vector<uint8_t> buffer_;

  Buffer() = default;

  SFT_DISALLOW_COPY_AND_ASSIGN(Buffer);
};

}  // namespace sft
