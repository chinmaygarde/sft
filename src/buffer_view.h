#pragma once

#include <memory>

#include "buffer.h"
#include "macros.h"

namespace sft {

struct BufferView {
  BufferView(Buffer& buffer, size_t offset = 0)
      : buffer_(buffer.AsShared()), offset_(offset) {}

  const uint8_t* GetData() const {
    if (!buffer_) {
      return nullptr;
    }
    return buffer_->GetData() + offset_;
  }

 private:
  std::shared_ptr<Buffer> buffer_;
  const size_t offset_;
};

}  // namespace sft
