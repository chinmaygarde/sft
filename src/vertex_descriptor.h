#pragma once

#include <cstddef>

namespace sft {

struct VertexDescriptor {
  size_t offset = 0;
  size_t stride = 0;
};

}  // namespace sft
