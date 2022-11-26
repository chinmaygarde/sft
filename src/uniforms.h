/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <map>

#include "buffer_view.h"
#include "image.h"
#include "macros.h"

namespace sft {

struct Uniforms {
  BufferView buffer;
  std::map<size_t, std::shared_ptr<Image>> images;

  Uniforms() = default;

  Uniforms(const BufferView& view) : buffer(view) {}
};

}  // namespace sft
