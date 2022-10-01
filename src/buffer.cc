#include "buffer.h"

namespace sft {

const uint8_t* Buffer::GetData() const {
  return buffer_.data();
}

std::shared_ptr<Buffer> Buffer::AsShared() {
  return shared_from_this();
}

size_t Buffer::GetLength() const {
  return buffer_.size();
}

}  // namespace sft
