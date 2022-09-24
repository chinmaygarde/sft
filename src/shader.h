#pragma once

#include <optional>

#include "geom.h"

namespace sft {

struct VertexInvocation;
struct FragmentInvocation;

class Shader {
 public:
  virtual ~Shader() = default;

  virtual size_t GetVaryingsSize() const = 0;

  virtual glm::vec3 ProcessVertex(
      const VertexInvocation& vertex_invocation) const = 0;

  virtual std::optional<Color> ProcessFragment(
      const FragmentInvocation& fragment_invocation) const = 0;
};

}  // namespace sft
