/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <optional>

#include "attachment.h"
#include "image.h"

namespace sft {

struct Paint {
  glm::vec4 color = kColorWhite;
  std::optional<ColorAttachmentDescriptor> color_desc;
  std::optional<DepthAttachmentDescriptor> depth_desc;
  std::optional<StencilAttachmentDescriptor> stencil_desc;
  std::shared_ptr<Image> image;
  uint32_t stencil_reference = 0;
};

}  // namespace sft
