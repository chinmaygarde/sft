#include "application.h"
#include "shaders/impeller_shader.h"
#include "shaders/simple_shader.h"
#include "shaders/texture_shader.h"

namespace sft {

Application::Application() {
  window_size_ = render_surface_size_;
  window_size_.x *= 2.0;

  rasterizer_ = std::make_unique<Rasterizer>(render_surface_size_);

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

void Application::OnTouchEvent(TouchEventType type, glm::vec2 pos) {}

bool Application::IsValid() const {
  return is_valid_;
}

glm::ivec2 Application::GetRenderSurfaceSize() const {
  return render_surface_size_;
}

}  // namespace sft
