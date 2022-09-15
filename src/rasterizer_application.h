#pragma once

#include "application.h"
#include "macros.h"

namespace sft {

class Rasterizer;

class RasterizerApplication final : public Application {
 public:
  RasterizerApplication();

  ~RasterizerApplication() override;

  using RasterizerCallback = std::function<bool(Rasterizer&)>;

  void SetRasterizerCallback(RasterizerCallback callback);

  bool Update() override;

  void OnTouchEvent(TouchEventType type, glm::vec2 pos) override;

 private:
  RasterizerCallback rasterizer_callback_;

  Rasterizer* GetRasterizer() const;

  SFT_DISALLOW_COPY_AND_ASSIGN(RasterizerApplication);
};

}  // namespace sft
