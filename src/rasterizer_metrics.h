#pragma once

#include "macros.h"

namespace sft {

struct RasterizerMetrics {
  size_t draw_count = 0;
  size_t primitive_count = 0;
  size_t face_culling = 0;
  size_t empty_primitive = 0;
  size_t scissor_culling = 0;
  size_t sample_point_culling = 0;
  size_t vertex_invocations = 0;
  size_t fragment_invocations = 0;

  void Display() const;

  void Reset() { ::memset(this, 0, sizeof(RasterizerMetrics)); }
};

}  // namespace sft
