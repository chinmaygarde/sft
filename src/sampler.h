/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include "geom.h"

namespace sft {

enum class WrapMode {
  kRepeat,
  kClamp,
  kMirror,
};

enum class Filter {
  kNearest,
  kLinear,
};

struct Sampler {
  WrapMode wrap_mode_s = WrapMode::kRepeat;
  WrapMode wrap_mode_t = WrapMode::kRepeat;
  Filter min_mag_filter = Filter::kNearest;
};

}  // namespace sft
