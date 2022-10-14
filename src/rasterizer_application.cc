#include "rasterizer_application.h"

#include "rasterizer.h"

namespace sft {

RasterizerApplication::RasterizerApplication(SampleCount sample_count)
    : Application(
          std::make_unique<Rasterizer>(glm::ivec2{1024, 768}, sample_count)) {}

RasterizerApplication::~RasterizerApplication() = default;

Rasterizer* RasterizerApplication::GetRasterizer() const {
  return static_cast<Rasterizer*>(renderer_.get());
}

bool RasterizerApplication::Update() {
  if (!rasterizer_callback_) {
    return false;
  }
  return rasterizer_callback_(*GetRasterizer());
}

void RasterizerApplication::OnTouchEvent(TouchEventType type, glm::vec2 pos) {
  //
}

void RasterizerApplication::SetRasterizerCallback(RasterizerCallback callback) {
  rasterizer_callback_ = std::move(callback);
}

Rasterizer* RasterizerApplication::GetHUDRasterizer() const {
  return GetRasterizer();
}

bool RasterizerApplication::OnWindowSizeChanged(glm::ivec2 size) {
  return GetRasterizer()->Resize(std::move(size));
}

}  // namespace sft
