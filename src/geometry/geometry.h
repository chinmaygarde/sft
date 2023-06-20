/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include "color.h"
#include "rect.h"
#include "size.h"
#include "types.h"

namespace sft {

template <class T>
constexpr T BarycentricInterpolation(const T& p1,
                                     const T& p2,
                                     const T& p3,
                                     const glm::vec3& bary) {
  return bary.x * p1 + bary.y * p2 + bary.z * p3;
}

size_t TileFactorForAvailableHardwareConcurrency();

}  // namespace sft
