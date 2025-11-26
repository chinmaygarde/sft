/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <cstdint>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace sft {

using Scalar = int32_t;
using ScalarF = float;

constexpr ScalarF kEpsilon = 1e-5;

constexpr glm::vec2 kSampleMidpoint = {0.5f, 0.5f};

}  // namespace sft
