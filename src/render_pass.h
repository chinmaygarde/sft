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

  virtual glm::ivec2 GetSize() const = 0;

  virtual bool IsValid() const = 0;

  virtual void Load() = 0;

  virtual void Store() = 0;
};

struct ColorPassAttachment final : public PassAttachment {
  glm::vec4 clear_color = {0.0, 0.0, 0.0, 1.0};
  std::shared_ptr<Framebuffer<Color>> texture;

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  bool IsValid() const override { return !!texture; }

  void Load() override {
    switch (load_action) {
      case LoadAction::kDontCare:
      case LoadAction::kLoad:
        break;
      case LoadAction::kClear:
        texture->Clear(clear_color);
        break;
    }
  }

  void Store() override {}
};

struct DepthPassAttachment : public PassAttachment {
  ScalarF clear_depth = 1.0;
  std::shared_ptr<Framebuffer<ScalarF>> texture;

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  bool IsValid() const override { return !!texture; }

  void Load() override {
    switch (load_action) {
      case LoadAction::kDontCare:
      case LoadAction::kLoad:
        break;
      case LoadAction::kClear:
        texture->Clear(clear_depth);
        break;
    }
  }

  void Store() override {}
};

struct StencilPassAttachment : public PassAttachment {
  uint32_t clear_stencil = 0;
  std::shared_ptr<Framebuffer<uint32_t>> texture;

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  bool IsValid() const override { return !!texture; }

  void Load() override {
    switch (load_action) {
      case LoadAction::kDontCare:
      case LoadAction::kLoad:
        break;
      case LoadAction::kClear:
        texture->Clear(clear_stencil);
        break;
    }
  }

  void Store() override {}
};

struct RenderPass {
  ColorPassAttachment color;
  DepthPassAttachment depth;
  StencilPassAttachment stencil;

  bool IsValid() const {
    if (!color.IsValid() || !depth.IsValid() || !stencil.IsValid()) {
      return false;
    }
    const auto texture_size = color.texture->GetSize();
    const auto depth_size = depth.texture->GetSize();
    const auto stencil_size = stencil.texture->GetSize();
    return texture_size == depth_size && texture_size == stencil_size;
  }

  [[nodiscard]] bool Begin() {
    if (!IsValid()) {
      return false;
    }
    color.Load();
    depth.Load();
    stencil.Load();
    return true;
  }

  [[nodiscard]] bool End() {
    if (!IsValid()) {
      return false;
    }
    color.Store();
    depth.Store();
    stencil.Store();
    return true;
  }
};

}  // namespace sft
