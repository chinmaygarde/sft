#include "rasterizer_application.h"

namespace sft {

RasterizerApplication::RasterizerApplication()
    : Application(std::make_unique<Rasterizer>(glm::ivec2{800, 600})) {
  pipeline_ = std::make_shared<Pipeline>();

  pipeline_->viewport = GetRasterizer()->GetSize();

  color_shader_ = std::make_shared<ColorShader>();

  pipeline_->shader = color_shader_;

  model_ = std::make_unique<Model>("assets/teapot/teapot.obj");
  if (!model_->IsValid()) {
    return;
  }

  GetRasterizer()->SetPipeline(pipeline_);
}

RasterizerApplication::~RasterizerApplication() = default;

Rasterizer* RasterizerApplication::GetRasterizer() const {
  return static_cast<Rasterizer*>(renderer_.get());
}

bool RasterizerApplication::Update() {
  GetRasterizer()->SetPipeline(pipeline_);

  GetRasterizer()->Clear(kColorWhite);

  pipeline_->blend_mode = BlendMode::kSourceOver;

  color_shader_->SetColor(kColorBlue.WithAlpha(128));

  auto tl = glm::vec3{-1 / 2.0, 1 / 2.0, 0};
  auto tr = glm::vec3{1 / 2.0, 1 / 2.0, 0};
  auto bl = glm::vec3{-1 / 2.0, -1 / 2.0, 0};
  auto br = glm::vec3{1 / 2.0, -1 / 2.0, 0};

  auto d = glm::vec3{0.25, 0.25, 0.0};

  GetRasterizer()->DrawTriangle(bl - d, br - d, tr - d);
  GetRasterizer()->DrawTriangle(tr - d, tl - d, bl - d);

  color_shader_->SetColor(kColorRed.WithAlpha(128));

  GetRasterizer()->DrawTriangle(bl + d, br + d, tr + d);
  GetRasterizer()->DrawTriangle(tr + d, tl + d, bl + d);

  // model_->RenderTo(*rasterizer_);

  return true;
}

void RasterizerApplication::OnTouchEvent(TouchEventType type, glm::vec2 pos) {
  switch (type) {
    case TouchEventType::kTouchDown:
      last_touch_ = pos;
      break;
    case TouchEventType::kTouchUp:
      last_touch_.reset();
      break;
    case TouchEventType::kTouchMove:
      if (!last_touch_.has_value()) {
        return;
      }
      touch_offset_ += (pos - last_touch_.value());
      last_touch_ = pos;
      break;
  }

  model_->SetTransformation(glm::rotate(glm::identity<glm::mat4>(),
                                        glm::radians(touch_offset_.x),
                                        glm::vec3(0.0f, -1.0f, 0.0f)));
}

}  // namespace sft
