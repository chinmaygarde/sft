/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <memory>

#include "attachment.h"
#include "geometry.h"
#include "image.h"
#include "macros.h"
#include "paint.h"
#include "rasterizer.h"
#include "shader.h"

namespace sft {

class CanvasContext;

std::shared_ptr<CanvasContext> CanvasContextCreate();

class Canvas {
 public:
  Canvas(std::shared_ptr<CanvasContext> context);

  void DrawRect(Rasterizer& rasterizer, Rect rect, const Paint& paint);

  void Translate(glm::vec2 tx);

  void Concat(const glm::mat4& xform);

 private:
  std::shared_ptr<CanvasContext> context_;
  glm::mat4 ctm_ = glm::identity<glm::mat4>();

  SFT_DISALLOW_COPY_AND_ASSIGN(Canvas);
};

}  // namespace sft
