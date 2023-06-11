/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <optional>

#include "geometry.h"

namespace sft {

struct VertexInvocation;
struct FragmentInvocation;

#define VTX(struct_member)                                 \
  inv.LoadVertexData<decltype(VertexData::struct_member)>( \
      offsetof(VertexData, struct_member))

#define VARYING_STORE(struct_member, value)            \
  inv.StoreVarying<decltype(Varyings::struct_member)>( \
      value, offsetof(Varyings, struct_member))
#define VARYING_LOAD(member) \
  inv.LoadVarying<decltype(Varyings::member)>(offsetof(Varyings, member))

#define UNIFORM(member) \
  inv.LoadUniform<decltype(Uniforms::member)>(offsetof(Uniforms, member))

#define FORWARD(vtx_member, var_member) \
  VARYING_STORE(var_member, VTX(vtx_member))

class Shader {
 public:
  Shader() = default;

  virtual ~Shader() = default;

  virtual size_t GetVaryingsSize() const = 0;

  virtual glm::vec4 ProcessVertex(const VertexInvocation& inv) const = 0;

  virtual glm::vec4 ProcessFragment(const FragmentInvocation& inv) const = 0;
};

}  // namespace sft
