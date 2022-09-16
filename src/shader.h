#pragma once

#include <optional>

#include "geom.h"

namespace sft {

struct VertexInvocation;
struct FragmentInvocation;

class Shader {
 public:
  virtual ~Shader() = default;

  virtual glm::vec3 ProcessVertex(
      const VertexInvocation& vertex_invocation) = 0;

  virtual std::optional<Color> ProcessFragment(
      const FragmentInvocation& fragment_invocation) = 0;
};

}  // namespace sft
