#pragma once

#include "geom.h"
#include "macros.h"

#include <cstring>

namespace sft {

struct RasterizerMetrics {
  glm::ivec2 area;
  size_t draw_count = 0;
  size_t primitive_count = 0;
  size_t primitives_processed = 0;
  size_t face_culling = 0;
  size_t empty_primitive = 0;
  size_t scissor_culling = 0;
  size_t sample_point_culling = 0;
  size_t early_fragment_test = 0;
  size_t vertex_invocations = 0;
  size_t fragment_invocations = 0;

  void Display() const;

  void Reset() { std::memset(this, 0, sizeof(RasterizerMetrics)); }
};

}  // namespace sft
