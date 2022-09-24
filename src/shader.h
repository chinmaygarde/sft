#pragma once

#include <optional>

#include "geom.h"

namespace sft {

struct VertexInvocation;
struct FragmentInvocation;

#define LOAD_VERTEX(struct_member)                         \
  inv.LoadVertexData<decltype(VertexData::struct_member)>( \
      offsetof(VertexData, struct_member))
#define STORE_VARYING(struct_member, value)            \
  inv.StoreVarying<decltype(Varyings::struct_member)>( \
      value, offsetof(Varyings, struct_member))
#define LOAD_VARYING(member) \
  inv.LoadVarying<decltype(Varyings::member)>(offsetof(Varyings, member))
#define LOAD_UNIFORM(member) \
  inv.LoadUniform<decltype(Uniforms::member)>(offsetof(Uniforms, member))
#define FORWARD(vtx_member, var_member) \
  STORE_VARYING(var_member, LOAD_VERTEX(vtx_member))

class Shader {
 public:
  Shader() = default;

  virtual ~Shader() = default;

  virtual size_t GetVaryingsSize() const = 0;

  virtual glm::vec3 ProcessVertex(const VertexInvocation& inv) const = 0;

  virtual std::optional<Color> ProcessFragment(
      const FragmentInvocation& inv) const = 0;
};

}  // namespace sft
