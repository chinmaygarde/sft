/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <array>
#include <optional>

#include "size.h"
#include "types.h"

namespace sft {

struct Rect {
  glm::vec2 origin = {0, 0};
  Size size;

  constexpr Rect() = default;

  constexpr Rect(glm::vec2 p_origin, Size p_size)
      : origin(p_origin), size(p_size) {}

  constexpr Rect(ScalarF x, ScalarF y, ScalarF width, ScalarF height)
      : origin({x, y}), size({width, height}) {}

  constexpr Rect(glm::vec2 sz) : origin({0, 0}), size(sz.x, sz.y) {}

  constexpr std::array<ScalarF, 4> GetLTRB() const {
    const auto left = std::min(origin.x, origin.x + size.width);
    const auto top = std::min(origin.y, origin.y + size.height);
    const auto right = std::max(origin.x, origin.x + size.width);
    const auto bottom = std::max(origin.y, origin.y + size.height);
    return {left, top, right, bottom};
  }

  constexpr glm::vec2 GetLT() const {
    const auto left = std::min(origin.x, origin.x + size.width);
    const auto top = std::min(origin.y, origin.y + size.height);
    return {left, top};
  }

  constexpr glm::vec2 GetRB() const {
    const auto right = std::max(origin.x, origin.x + size.width);
    const auto bottom = std::max(origin.y, origin.y + size.height);
    return {right, bottom};
  }

  constexpr static Rect MakeLTRB(ScalarF left,
                                 ScalarF top,
                                 ScalarF right,
                                 ScalarF bottom) {
    return Rect(left, top, right - left, bottom - top);
  }

  constexpr std::optional<Rect> Intersection(const Rect& o) const {
    auto this_ltrb = GetLTRB();
    auto other_ltrb = o.GetLTRB();
    auto intersection =
        Rect::MakeLTRB(std::max(this_ltrb[0], other_ltrb[0]),  //
                       std::max(this_ltrb[1], other_ltrb[1]),  //
                       std::min(this_ltrb[2], other_ltrb[2]),  //
                       std::min(this_ltrb[3], other_ltrb[3])   //
        );
    if (intersection.size.IsEmpty()) {
      return std::nullopt;
    }
    return intersection;
  }
};

}  // namespace sft
