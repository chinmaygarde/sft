#pragma once

#include "framebuffer.h"
#include "geom.h"
#include "macros.h"
#include "rasterizer.h"

namespace sft {

enum class LoadAction {
  kDontCare,
  kLoad,
  kClear,
};

enum class StoreAction {
  kDontCare,
  kStore,
};

struct PassAttachment {
  LoadAction load_action = LoadAction::kDontCare;
  StoreAction store_action = StoreAction::kDontCare;
  std::shared_ptr<FramebufferBase> texture;

  bool IsValid() const { return !!texture; }

  virtual void Load() = 0;

  virtual void Store() = 0;
};

struct ColorPassAttachment final : public PassAttachment {
  glm::vec4 clear_color = {0.0, 0.0, 0.0, 1.0};

  void Load() override {}

  void Store() override {}
};

struct DepthPassAttachment : public PassAttachment {
  ScalarF clear_depth = 1.0;

  void Load() override {}

  void Store() override {}
};

struct StencilPassAttachment : public PassAttachment {
  uint32_t clear_stencil = 0;

  void Load() override {}

  void Store() override {}
};

struct RenderPass {
  ColorPassAttachment color;
  DepthPassAttachment depth;
  StencilPassAttachment stencil;

  // bool IsValid() const {
  //   if (!color.IsValid() || !depth.IsValid() || !stencil.IsValid()) {
  //     return false;
  //   }
  //   auto size = color.texture->GetSize();
  //   if (!glm::equal(size, depth.texture->GetSize())) {
  //     return false;
  //   }

  //   if (!glm::equal(size, stencil.texture->GetSize())) {
  //     return false;
  //   }
  // }

  void Begin() {}

  void End() {}
};

}  // namespace sft
