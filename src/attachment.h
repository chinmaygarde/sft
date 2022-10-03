#pragma once

#include "blend.h"
#include "geom.h"
#include "macros.h"

namespace sft {

enum class CompareFunction {
  /// Comparison test never passes.
  kNever,
  /// Comparison test passes always passes.
  kAlways,
  /// Comparison test passes if new_value < current_value.
  kLess,
  /// Comparison test passes if new_value == current_value.
  kEqual,
  /// Comparison test passes if new_value <= current_value.
  kLessEqual,
  /// Comparison test passes if new_value > current_value.
  kGreater,
  /// Comparison test passes if new_value != current_value.
  kNotEqual,
  /// Comparison test passes if new_value >= current_value.
  kGreaterEqual,
};

template <class T>
constexpr bool CompareFunctionPasses(CompareFunction comp,
                                     const T& new_value,
                                     const T& current_value) {
  switch (comp) {
    case CompareFunction::kNever:
      return false;
    case CompareFunction::kAlways:
      return true;
    case CompareFunction::kLess:
      return new_value < current_value;
    case CompareFunction::kEqual:
      return new_value == current_value;
    case CompareFunction::kLessEqual:
      return new_value <= current_value;
    case CompareFunction::kGreater:
      return new_value > current_value;
    case CompareFunction::kNotEqual:
      return new_value != current_value;
    case CompareFunction::kGreaterEqual:
      return new_value >= current_value;
  }
  return true;
}

enum class StencilOperation {
  /// Don't modify the current stencil value.
  kKeep,
  /// Reset the stencil value to zero.
  kZero,
  /// Reset the stencil value to the reference value.
  kSetToReferenceValue,
  /// Increment the current stencil value by 1. Clamp it to the maximum.
  kIncrementClamp,
  /// Decrement the current stencil value by 1. Clamp it to zero.
  kDecrementClamp,
  /// Perform a logical bitwise invert on the current stencil value.
  kInvert,
  /// Increment the current stencil value by 1. If at maximum, set to zero.
  kIncrementWrap,
  /// Decrement the current stencil value by 1. If at zero, set to maximum.
  kDecrementWrap,
};

struct ColorAttachmentDescriptor {
  BlendDescriptor blend;
};

struct DepthAttachmentDescriptor {
  //----------------------------------------------------------------------------
  /// Indicates if the depth test must be performed. If disabled, all access to
  /// the depth buffer is disabled.
  ///
  bool depth_test_enabled = false;
  //----------------------------------------------------------------------------
  /// Indicates how to compare the value with that in the depth buffer.
  ///
  CompareFunction depth_compare = CompareFunction::kLessEqual;
  //----------------------------------------------------------------------------
  /// Indicates when writes must be performed to the depth buffer.
  ///
  bool depth_write_enabled = true;
};

struct StencilAttachmentDescriptor {
  //----------------------------------------------------------------------------
  /// Indicates if the stencil test must be performed. If disabled, all access
  /// to the stencil buffer is disabled.
  ///
  bool stencil_test_enabled = false;
  //----------------------------------------------------------------------------
  /// Indicates the operation to perform between the reference value and the
  /// value in the stencil buffer. Both values have the read_mask applied to
  /// them before performing this operation.
  ///
  CompareFunction stencil_compare = CompareFunction::kAlways;
  //----------------------------------------------------------------------------
  /// Indicates what to do when the stencil test has failed.
  ///
  StencilOperation stencil_failure = StencilOperation::kKeep;
  //----------------------------------------------------------------------------
  /// Indicates what to do when the stencil test passes but the depth test
  /// fails.
  ///
  StencilOperation depth_failure = StencilOperation::kKeep;
  //----------------------------------------------------------------------------
  /// Indicates what to do when both the stencil and depth tests pass.
  ///
  StencilOperation depth_stencil_pass = StencilOperation::kKeep;
  //----------------------------------------------------------------------------
  /// The mask applied to the reference and stencil buffer values before
  /// performing the stencil_compare operation.
  ///
  uint32_t read_mask = ~0;
  //----------------------------------------------------------------------------
  /// The mask applied to the new stencil value before it is written into the
  /// stencil buffer.
  ///
  uint32_t write_mask = ~0;
};

}  // namespace sft
