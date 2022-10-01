#include "buffer_view.h"

#include "buffer.h"

namespace sft {

BufferView::BufferView(Buffer& buffer)
    : buffer_(buffer.AsShared()), offset_(0u), length_(buffer.GetLength()) {}

BufferView::BufferView(Buffer& buffer, size_t offset, size_t length)
    : buffer_(buffer.AsShared()), offset_(offset), length_(length) {}

const uint8_t* BufferView::GetData() const {
  if (!buffer_) {
    return nullptr;
  }
  return buffer_->GetData() + offset_;
}

}  // namespace sft
