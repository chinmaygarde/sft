#include "application.h"

#include "shaders/simple_shader.h"
#include "shaders/texture_shader.h"

namespace sft {

Application::Application(std::shared_ptr<Renderer> renderer)
    : renderer_(std::move(renderer)) {
  SFT_ASSERT(renderer_ && renderer_->GetPixels());

  window_size_ = renderer_->GetSize();
  window_size_.x *= 2.0;

#ifndef NDEBUG
#define SFT_DEBUG_TITLE "Debug Build"
#else
#define SFT_DEBUG_TITLE "Release Build"
#endif

  sdl_window_ = ::SDL_CreateWindow(
      "SFT Sandbox (" SFT_DEBUG_TITLE ") (Press \"q\" or ESC to quit)",  //
      SDL_WINDOWPOS_CENTERED,                                            //
      SDL_WINDOWPOS_CENTERED,                                            //
      window_size_.x,                                                    //
      window_size_.y,                                                    //
      0                                                                  //
  );
  if (!sdl_window_) {
    return;
  }

  sdl_renderer_ = ::SDL_CreateRenderer(sdl_window_, -1, 0);
  if (!sdl_renderer_) {
    return;
  }

  is_valid_ = true;
}

Application::~Application() {
  if (sdl_window_) {
    ::SDL_DestroyWindow(sdl_window_);
  }
  if (sdl_renderer_) {
    ::SDL_DestroyRenderer(sdl_renderer_);
  }
}

bool Application::Render() {
  if (!is_valid_) {
    return false;
  }

  if (!Update()) {
    return false;
  }

  const auto size = renderer_->GetSize();

  SDLTextureNoCopyCaster color_attachment(sdl_renderer_,                 //
                                          renderer_->GetPixels(),        //
                                          size.x,                        //
                                          size.y,                        //
                                          renderer_->GetBytesPerPixel()  //
  );

  SDL_Rect dest = {};
  dest.x = 0;
  dest.y = 0;
  dest.w = size.x;
  dest.h = size.y;
  if (::SDL_RenderCopyEx(sdl_renderer_,       //
                         color_attachment,    //
                         nullptr,             //
                         &dest,               //
                         180,                 //
                         NULL,                //
                         SDL_FLIP_HORIZONTAL  //
                         ) != 0) {
    return false;
  }
  dest.x += dest.w;

  SDLTextureNoCopyCaster depth_attachment(sdl_renderer_,                      //
                                          renderer_->GetDepthPixels(),        //
                                          size.x,                             //
                                          size.y,                             //
                                          renderer_->GetDepthBytesPerPixel()  //
  );
  if (depth_attachment && ::SDL_RenderCopyEx(sdl_renderer_,       //
                                             depth_attachment,    //
                                             nullptr,             //
                                             &dest,               //
                                             180,                 //
                                             NULL,                //
                                             SDL_FLIP_HORIZONTAL  //
                                             ) != 0) {
    return false;
  }

  ::SDL_RenderPresent(sdl_renderer_);
  return true;
}

void Application::OnTouchEvent(TouchEventType type, glm::vec2 pos) {}

bool Application::IsValid() const {
  return is_valid_;
}

bool Application::Update() {
  return true;
}

}  // namespace sft
