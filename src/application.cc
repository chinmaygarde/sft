#include "application.h"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace sft {

using MillisecondsF = std::chrono::duration<double, std::milli>;

std::string CreateWindowTitle(MillisecondsF frame_time) {
  std::stringstream stream;
  stream.precision(2);
  stream.setf(std::ios::fixed, std::ios::floatfield);
  stream << "SFT Sandbox (";
#ifndef NDEBUG
  stream << "Debug Build";
#else
  stream << "Release Build";
#endif
  stream << ") (";
  stream << frame_time.count();
  stream << " ms) (Press \"q\" or ESC to quit)";
  return stream.str();
}

Application::Application(std::shared_ptr<Renderer> renderer)
    : renderer_(std::move(renderer)) {
  SFT_ASSERT(renderer_ && renderer_->GetPixels());

  window_size_ = renderer_->GetSize();
  window_size_.x *= 2.0;

  sdl_window_ = ::SDL_CreateWindow(CreateWindowTitle(MillisecondsF{0}).c_str(),
                                   SDL_WINDOWPOS_CENTERED,  //
                                   SDL_WINDOWPOS_CENTERED,  //
                                   window_size_.x,          //
                                   window_size_.y,          //
                                   0                        //
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
  const auto result = OnRender();
  const auto now = std::chrono::high_resolution_clock::now();
  if (std::chrono::duration_cast<MillisecondsF>(now - last_title_update_)
          .count() > 500) {
    ::SDL_SetWindowTitle(
        sdl_window_,
        CreateWindowTitle(
            std::chrono::duration_cast<MillisecondsF>(last_update_duration_))
            .c_str());
    last_title_update_ = now;
  }
  return result;
}

void Application::OnTouchEvent(TouchEventType type, glm::vec2 pos) {}

bool Application::IsValid() const {
  return is_valid_;
}

bool Application::Update() {
  return true;
}

bool Application::OnRender() {
  if (!is_valid_) {
    return false;
  }

  const auto update_start = std::chrono::high_resolution_clock::now();
  if (!Update()) {
    return false;
  }
  const auto update_end = std::chrono::high_resolution_clock::now();

  last_update_duration_ = update_end - update_start;

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

}  // namespace sft
