#pragma once

#include "application.h"
#include "framebuffer.h"
#include "macros.h"

namespace sft {

class Rasterizer;

class RasterizerApplication final : public Application {
 public:
  RasterizerApplication(SampleCount sample_count = SampleCount::kOne);

  ~RasterizerApplication() override;

  using RasterizerCallback = std::function<bool(Rasterizer&)>;

  void SetRasterizerCallback(RasterizerCallback callback);

  bool Update() override;

  void OnTouchEvent(TouchEventType type, glm::vec2 pos) override;

  bool OnWindowSizeChanged(glm::ivec2 size) override;

  Rasterizer* GetHUDRasterizer() const override;

 private:
  RasterizerCallback rasterizer_callback_;

  Rasterizer* GetRasterizer() const;

  SFT_DISALLOW_COPY_AND_ASSIGN(RasterizerApplication);
};

}  // namespace sft
