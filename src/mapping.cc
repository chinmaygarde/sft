#include "mapping.h"

namespace sft {

std::unique_ptr<Mapping> Mapping::MakeWithCopy(const uint8_t* buffer,
                                               size_t size) {
  auto copied = ::malloc(size);
  if (!copied) {
    return nullptr;
  }
  ::memmove(copied, buffer, size);
  return std::make_unique<Mapping>(reinterpret_cast<const uint8_t*>(copied),
                                   size, [copied]() { ::free(copied); });
}

Mapping::Mapping(const uint8_t* buffer,
                 size_t size,
                 std::function<void(void)> on_done)
    : buffer_(buffer), size_(size), on_done_(std::move(on_done)) {}

Mapping::~Mapping() {
  if (on_done_) {
    on_done_();
  }
}

const uint8_t* Mapping::GetBuffer() const {
  return buffer_;
}

size_t Mapping::GetSize() const {
  return size_;
}

}  // namespace sft
