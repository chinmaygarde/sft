/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include "types.h"

namespace sft {

struct Size {
  ScalarF width = 0.0;
  ScalarF height = 0.0;

  constexpr Size() = default;

  constexpr Size(ScalarF p_width, ScalarF p_height)
      : width(p_width), height(p_height) {}

  constexpr bool IsEmpty() const { return width * height <= 0.0; }

  constexpr ScalarF GetArea() const { return width * height; }
};

}  // namespace sft
