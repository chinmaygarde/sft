#include "application.h"

namespace sft {

Application::Application() {
  glm::ivec2 render_surface_size = {800, 600};
  window_size_ = render_surface_size;
  window_size_.x *= 2.0;

  rasterizer_ = std::make_unique<Rasterizer>(render_surface_size);

  if (!rasterizer_ || !rasterizer_->GetPixels()) {
    return;
  }

#ifndef NDEBUG
#define SFT_DEBUG_TITLE "Debug Build"
#else
#define SFT_DEBUG_TITLE "Release Build"
#endif

  window_ = ::SDL_CreateWindow("SFT Sandbox (" SFT_DEBUG_TITLE
                               ") (Press \"q\" or ESC to quit)",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               window_size_.x, window_size_.y, 0);
  if (!window_) {
    return;
  }

  renderer_ = ::SDL_CreateRenderer(window_, -1, 0);
  if (!renderer_) {
    return;
  }

  model_ = std::make_unique<Model>("assets/teapot/teapot.obj");
  if (!model_->IsValid()) {
    return;
  }

  is_valid_ = true;
}

Application::~Application() {
  if (window_) {
    ::SDL_DestroyWindow(window_);
  }
  if (renderer_) {
    ::SDL_DestroyRenderer(renderer_);
  }
}

bool Application::Render() {
  if (!is_valid_) {
    return false;
  }

  if (!Update()) {
    return false;
  }

  const auto size = rasterizer_->GetSize();

  SDLTextureNoCopyCaster color_attachment(renderer_,                       //
                                          rasterizer_->GetPixels(),        //
                                          size.x,                          //
                                          size.y,                          //
                                          rasterizer_->GetBytesPerPixel()  //

  );
  SDLTextureNoCopyCaster depth_attachment(
      renderer_,                            //
      rasterizer_->GetDepthPixels(),        //
      size.x,                               //
      size.y,                               //
      rasterizer_->GetDepthBytesPerPixel()  //

  );
  SDL_Rect dest = {};
  dest.x = 0;
  dest.y = 0;
  dest.w = size.x;
  dest.h = size.y;
  if (::SDL_RenderCopyEx(renderer_, color_attachment, nullptr, &dest, 180, NULL,
                         SDL_FLIP_HORIZONTAL) != 0) {
    return false;
  }
  dest.x += dest.w;
  if (::SDL_RenderCopyEx(renderer_, depth_attachment, nullptr, &dest, 180, NULL,
                         SDL_FLIP_HORIZONTAL) != 0) {
    return false;
  }
  ::SDL_RenderPresent(renderer_);
  return true;
}

bool Application::Update() {
  rasterizer_->SetDepthTestsEnabled(false);
  rasterizer_->Clear(kColorWhite);
  rasterizer_->SetDepthTestsEnabled(true);
  // rasterizer_->DrawTriangle({-1, -1, -1},  //
  //                           {1, -1, -1},   //
  //                           {0, 1, 1},     //
  //                           kColorRed);
  // rasterizer_->DrawTriangle({-1, 1, -1},  //
  //                           {1, 1, -1},   //
  //                           {0, -1, 1},   //
  //                           kColorGreen);
  model_->RenderTo(*rasterizer_);

  return true;
}

void Application::OnTouchEvent(TouchEventType type, glm::vec2 pos) {
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
