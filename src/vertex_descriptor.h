#pragma once

#include <cstddef>

namespace sft {

enum class IndexType {
  kUInt32,
  kUInt16,
};

struct VertexDescriptor {
  size_t offset = 0;
  size_t stride = 0;
  IndexType index_type = IndexType::kUInt32;
};

}  // namespace sft
