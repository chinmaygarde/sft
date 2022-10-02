#pragma once

#include "geom.h"
#include "macros.h"

namespace sft {

enum class BlendOp {
  kAdd,
  kSubtract,
  kReverseSubtract,
  kMin,
  kMax,
};

enum class BlendFactor {
  kZero,
  kOne,
  kSourceColor,
  kOneMinusSourceColor,
  kSourceAlpha,
  kOneMinusSourceAlpha,
  kDestinationColor,
  kOneMinusDestinationColor,
  kDestinationAlpha,
  kOneMinusDestinationAlpha,
  kSourceAlphaSaturated,
};

/// Specify how new (src) fragments should be combined with fragments already in
/// the framebuffer (dst).
///
/// ```
/// if (blending_enabled) {
///   new_color.rgb = (src_color_blend_factor * src_color.rgb)
///                             <color_blend_op>
///                     (dst_color_blend_factor * dst_color.rgb);
///   new_color.a = (src_alpha_blend_factor * src_color.a)
///                             <alpha_blend_op>
///                     (dst_alpha_blend_factor * dst_color.a);
/// } else {
///   new_color = src_color;
/// }
/// IMPORTANT: The write mask is applied irrespective of whether
///            blending_enabled is set.
/// new_color = new_color & write_mask;
/// ```
struct BlendDescriptor {
  bool enabled = false;

  BlendFactor src_color_fac = BlendFactor::kSourceAlpha;
  BlendOp color_op = BlendOp::kAdd;
  BlendFactor dst_color_fac = BlendFactor::kOneMinusSourceAlpha;

  BlendFactor src_alpha_fac = BlendFactor::kSourceAlpha;
  BlendOp alpha_op = BlendOp::kAdd;
  BlendFactor dst_alpha_fac = BlendFactor::kOneMinusSourceAlpha;

  static constexpr glm::vec3 ApplyFactorColor(BlendFactor factor,
                                              glm::vec4 src,
                                              glm::vec4 dst) {
    switch (factor) {
      case BlendFactor::kZero:
        return glm::vec3{0.0};
      case BlendFactor::kOne:
        return glm::vec3{1.0};
      case BlendFactor::kSourceColor:
        return glm::vec3{src};
      case BlendFactor::kOneMinusSourceColor:
        return glm::vec3{1.0} - glm::vec3{src};
      case BlendFactor::kSourceAlpha:
        return glm::vec3{src.a};
      case BlendFactor::kOneMinusSourceAlpha:
        return glm::vec3{1.0} - glm::vec3{src.a};
      case BlendFactor::kDestinationColor:
        return glm::vec3{dst};
      case BlendFactor::kOneMinusDestinationColor:
        return glm::vec3{1.0} - glm::vec3{dst};
      case BlendFactor::kDestinationAlpha:
        return glm::vec3{dst.a};
      case BlendFactor::kOneMinusDestinationAlpha:
        return glm::vec3{1.0} - glm::vec3{dst.a};
      case BlendFactor::kSourceAlphaSaturated:
        return glm::min(glm::vec3{src.a}, glm::vec3{1} - glm::vec3{dst.a});
    }
    return {};
  }

  static constexpr ScalarF ApplyFactorAlpha(BlendFactor factor,
                                            glm::vec4 src,
                                            glm::vec4 dst) {
    switch (factor) {
      case BlendFactor::kZero:
        return 0.0;
      case BlendFactor::kOne:
        return 1.0;
      case BlendFactor::kSourceColor:
        return src.a;
      case BlendFactor::kOneMinusSourceColor:
        return 1.0 - src.a;
      case BlendFactor::kSourceAlpha:
        return src.a;
      case BlendFactor::kOneMinusSourceAlpha:
        return 1.0 - src.a;
      case BlendFactor::kDestinationColor:
        return dst.a;
      case BlendFactor::kOneMinusDestinationColor:
        return 1.0 - dst.a;
      case BlendFactor::kDestinationAlpha:
        return dst.a;
      case BlendFactor::kOneMinusDestinationAlpha:
        return 1.0 - dst.a;
      case BlendFactor::kSourceAlphaSaturated:
        return 1;
    }
    return 0;
  }

  template <class T>
  static constexpr T ApplyOp(BlendOp op, const T& src, const T& dst) {
    switch (op) {
      case BlendOp::kAdd:
        return src + dst;
      case BlendOp::kSubtract:
        return src - dst;
      case BlendOp::kReverseSubtract:
        return dst - src;
      case BlendOp::kMin:
        return glm::min(src, dst);
      case BlendOp::kMax:
        return glm::max(src, dst);
    }
    return src + dst;
  }

  static constexpr glm::vec3 RGB(const glm::vec4& color) {
    return glm::vec3{color};
  }

  constexpr glm::vec4 Blend(glm::vec4 src, glm::vec4 dst) const {
    if (!enabled) {
      return src;
    }
    auto color =
        ApplyOp(color_op,                                              //
                ApplyFactorColor(src_color_fac, src, dst) * RGB(src),  //
                ApplyFactorColor(dst_color_fac, src, dst) * RGB(dst)   //
        );
    auto alpha = ApplyOp(alpha_op,                                           //
                         ApplyFactorAlpha(src_color_fac, src, dst) * src.a,  //
                         ApplyFactorAlpha(dst_color_fac, src, dst) * dst.a   //
    );
    return glm::vec4{color.x, color.y, color.z, alpha};
  }
};

}  // namespace sft
