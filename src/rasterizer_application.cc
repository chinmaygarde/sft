#include "rasterizer_application.h"

#include "rasterizer.h"

namespace sft {

RasterizerApplication::RasterizerApplication()
    : Application(std::make_unique<Rasterizer>(glm::ivec2{1024, 768})) {}

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

}  // namespace sft
