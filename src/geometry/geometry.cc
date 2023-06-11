/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "geometry.h"

#include <thread>

namespace sft {

size_t TileFactorForAvailableHardwareConcurrency() {
  const auto cores = std::thread::hardware_concurrency();
  auto factor = glm::log2(static_cast<ScalarF>(cores));
  factor = std::ceil(factor);
  factor += 1.0f;
  factor = std::max(2.0f, factor);
  return factor;
}

}  // namespace sft
