#include "application.h"

#include <iomanip>
#include <sstream>

#include "imgui_impl_sft.h"
#include "rasterizer.h"
#include "timing.h"

namespace sft {

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
    : renderer_(std::move(renderer)), launch_time_(Clock::now()) {
  SFT_ASSERT(renderer_ && renderer_->GetPixels());

  window_size_ = renderer_->GetSize();

  Uint32 window_flags = 0;

  // window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;

  sdl_window_ = ::SDL_CreateWindow(CreateWindowTitle(MillisecondsF{0}).c_str(),
                                   SDL_WINDOWPOS_CENTERED,  //
                                   SDL_WINDOWPOS_CENTERED,  //
                                   window_size_.x,          //
                                   window_size_.y,          //
                                   window_flags             //
  );
  if (!sdl_window_) {
    return;
  }

  sdl_renderer_ = ::SDL_CreateRenderer(sdl_window_, -1, 0);
  if (!sdl_renderer_) {
    return;
  }

  ImGui::CreateContext();
  auto result = ImGui_ImplSFT_Init(sdl_window_, sdl_renderer_);
  if (!result) {
    return;
  }

  is_valid_ = true;
}

Application::~Application() {
  ImGui_ImplSFT_Shutdown();
  ImGui::DestroyContext();

  if (sdl_window_) {
    ::SDL_DestroyWindow(sdl_window_);
  }
  if (sdl_renderer_) {
    ::SDL_DestroyRenderer(sdl_renderer_);
  }
}

bool Application::Render() {
  const auto result = OnRender();

  const auto now = Clock::now();
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

  auto rasterizer = GetHUDRasterizer();

  const auto update_start = Clock::now();

  ImGui_ImplSFT_NewFrame();
  ImGui::NewFrame();

  if (rasterizer) {
    rasterizer->ResetMetrics();
  }

  if (!Update()) {
    return false;
  }

  if (rasterizer) {
    rasterizer->GetMetrics().Display();
    rasterizer->ResetMetrics();
  }

  ImGui::Render();

  if (rasterizer) {
    ImGui_ImplSFT_RenderDrawData(rasterizer, ImGui::GetDrawData());
  }

  const auto update_end = Clock::now();

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

  ::SDL_RenderPresent(sdl_renderer_);
  return true;
}

SecondsF Application::GetTimeSinceLaunch() const {
  return Clock::now() - launch_time_;
}

Rasterizer* Application::GetHUDRasterizer() const {
  return nullptr;
}

}  // namespace sft
