#pragma once

#include "application.h"
#include "macros.h"
#include "model.h"
#include "shaders/color_shader.h"
#include "shaders/impeller_shader.h"

namespace sft {

class RasterizerApplication final : public Application {
 public:
  RasterizerApplication();

  ~RasterizerApplication() override;

  bool Update() override;

  void OnTouchEvent(TouchEventType type, glm::vec2 pos) override;

 private:
  std::unique_ptr<Model> model_;
  glm::vec2 touch_offset_;
  std::optional<glm::vec2> last_touch_;
  std::shared_ptr<ColorShader> color_shader_;
  std::shared_ptr<Pipeline> pipeline_;

  Rasterizer* GetRasterizer() const;

  SFT_DISALLOW_COPY_AND_ASSIGN(RasterizerApplication);
};

}  // namespace sft
