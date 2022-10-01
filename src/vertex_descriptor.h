#pragma once

#include <cstddef>

namespace sft {

enum class IndexType {
  kUInt32,
  kUInt16,
};

enum class VertexFormat {
  kFloat2,  // like glm::vec2
  kFloat3,  // like glm::vec3
};

struct VertexDescriptor {
  size_t offset = 0;
  size_t stride = 0;
  IndexType index_type = IndexType::kUInt32;
  VertexFormat vertex_format = VertexFormat::kFloat3;
};

}  // namespace sft
