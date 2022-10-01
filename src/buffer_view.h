#pragma once

#include <memory>

#include "macros.h"

namespace sft {

class Buffer;

struct BufferView {
  BufferView(Buffer& buffer);

  BufferView(Buffer& buffer, size_t offset, size_t length);

  const uint8_t* GetData() const;

 private:
  std::shared_ptr<Buffer> buffer_;
  const size_t offset_;
  const size_t length_;
};

}  // namespace sft
