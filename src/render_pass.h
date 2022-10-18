#pragma once

#include "geom.h"
#include "macros.h"
#include "texture.h"

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
  LoadAction load_action = LoadAction::kClear;
  StoreAction store_action = StoreAction::kDontCare;

  virtual glm::ivec2 GetSize() const = 0;

  virtual bool IsValid() const = 0;

  virtual void Load() = 0;

  virtual void Store() = 0;
};

struct ColorPassAttachment final : public PassAttachment {
  glm::vec4 clear_color = {0.0, 0.0, 0.0, 1.0};
  std::shared_ptr<Texture<Color>> texture;
  std::shared_ptr<Texture<Color>> resolve;

  ColorPassAttachment(glm::ivec2 size, SampleCount sample_count) {
    texture = std::make_shared<Texture<Color>>(size, sample_count);
    if (sample_count != SampleCount::kOne) {
      resolve = std::make_shared<Texture<Color>>(size, SampleCount::kOne);
    }
  }

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  [[nodiscard]] bool Resize(const glm::ivec2& size) {
    if (!IsValid()) {
      return false;
    }
    if (size == GetSize()) {
      return true;
    }
    if (resolve) {
      return resolve->Resize(size);
    }
    return texture->Resize(size);
  }

  [[nodiscard]] bool SetSampleCount(SampleCount count) {
    if (!IsValid()) {
      return false;
    }
    return texture->UpdateSampleCount(count);
  }

  bool IsValid() const override {
    if (!texture) {
      return false;
    }
    if (texture->GetSampleCount() != SampleCount::kOne) {
      return resolve && resolve->GetSampleCount() == SampleCount::kOne;
    }
    return true;
  }

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
  std::shared_ptr<Texture<ScalarF>> texture;

  DepthPassAttachment(const glm::ivec2& size) {
    texture = std::make_shared<Texture<ScalarF>>(size);
  }

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  bool IsValid() const override { return !!texture; }

  [[nodiscard]] bool Resize(const glm::ivec2& size) {
    if (!IsValid()) {
      return false;
    }
    if (size == GetSize()) {
      return true;
    }
    return texture->Resize(size);
  }

  [[nodiscard]] bool SetSampleCount(SampleCount count) {
    if (!IsValid()) {
      return false;
    }
    return texture->UpdateSampleCount(count);
  }

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
  std::shared_ptr<Texture<uint8_t>> texture;

  StencilPassAttachment(const glm::ivec2& size) {
    texture = std::make_shared<Texture<uint8_t>>(size);
  }

  glm::ivec2 GetSize() const override { return texture->GetSize(); }

  bool IsValid() const override { return !!texture; }

  [[nodiscard]] bool Resize(const glm::ivec2& size) {
    if (!IsValid()) {
      return false;
    }
    if (size == GetSize()) {
      return true;
    }
    return texture->Resize(size);
  }

  [[nodiscard]] bool SetSampleCount(SampleCount count) {
    if (!IsValid()) {
      return false;
    }
    return texture->UpdateSampleCount(count);
  }

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

  RenderPass(const glm::ivec2& size, SampleCount sample_count)
      : color(size, sample_count), depth(size), stencil(size) {}

  [[nodiscard]] bool Resize(const glm::ivec2& size) {
    return color.Resize(size) && depth.Resize(size) && stencil.Resize(size);
  }

  [[nodiscard]] bool SetSampleCount(SampleCount count) {
    return color.SetSampleCount(count) && depth.SetSampleCount(count) &&
           stencil.SetSampleCount(count);
  }

  glm::ivec2 GetSize() const {
    if (!color.IsValid()) {
      return {};
    }
    return color.GetSize();
  }

  bool IsValid() const {
    if (!color.IsValid() || !depth.IsValid() || !stencil.IsValid()) {
      return false;
    }
    const auto texture_size = color.texture->GetSize();
    const auto depth_size = depth.texture->GetSize();
    const auto stencil_size = stencil.texture->GetSize();
    return texture_size == depth_size && texture_size == stencil_size;
  }

  bool Begin() {
    color.Load();
    depth.Load();
    stencil.Load();
    return true;
  }

  bool End() {
    color.Store();
    depth.Store();
    stencil.Store();
    return true;
  }
};

}  // namespace sft
