/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "application.h"

#include <iomanip>
#include <sstream>

#include "imgui_impl_sft.h"
#include "rasterizer.h"
#include "timing.h"

namespace sft {

std::string CreateWindowTitle(MillisecondsF frame_time,
                              const std::string& title) {
  std::stringstream stream;
  stream.precision(2);
  stream.setf(std::ios::fixed, std::ios::floatfield);
  if (!title.empty()) {
    stream << title;
  } else {
    stream << "SFT Sandbox";
  }
  stream << " ";
#ifndef NDEBUG
  stream << "DEBUG";
#else
  stream << "RELEASE";
#endif
  stream << " (";
  stream << frame_time.count();
  stream << " ms)";
  return stream.str();
}

Application::Application(glm::ivec2 size, SampleCount sample_count)
    : scheduler_(std::make_unique<marl::Scheduler>(
          marl::Scheduler::Config::allCores())),
      rasterizer_(std::make_shared<Rasterizer>(size, sample_count)),
      launch_time_(Clock::now()) {
  scheduler_->bind();

  window_size_ = rasterizer_->GetSize();

  Uint32 window_flags = 0;

  window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
  window_flags |= SDL_WINDOW_RESIZABLE;

  sdl_window_ =
      ::SDL_CreateWindow(CreateWindowTitle(MillisecondsF{0}, "").c_str(),
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

  // The size of the renderer and that of the rasterizer may differ in case of
  // Hi-DPI setups. Get the actual size and scale.
  glm::ivec2 output_size;
  if (::SDL_GetRendererOutputSize(sdl_renderer_,   //
                                  &output_size.x,  //
                                  &output_size.y   //
                                  ) != 0) {
    return;
  }

  if (!rasterizer_->Resize(output_size)) {
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
  scheduler_->unbind();
}

bool Application::Render() {
  const auto result = OnRender();

  const auto now = Clock::now();
  if (std::chrono::duration_cast<MillisecondsF>(now - last_title_update_)
          .count() > 500) {
    ::SDL_SetWindowTitle(
        sdl_window_,
        CreateWindowTitle(
            std::chrono::duration_cast<MillisecondsF>(last_update_duration_),
            title_)
            .c_str());
    last_title_update_ = now;
  }
  return result;
}

bool Application::IsValid() const {
  return is_valid_;
}

bool Application::Update() {
  if (!rasterizer_callback_) {
    return false;
  }
  return rasterizer_callback_(*rasterizer_);
}

static void DisplayMetrics(const RasterizerMetrics& m) {
  ImGui::Begin("Rasterizer Metrics");

  ImGui::Text("Size: %d x %d", m.area.x, m.area.y);
  ImGui::Text("Draw Count: %zu", m.draw_count);
  ImGui::Text("Primitives: %zu", m.primitive_count);
  ImGui::Text("Primitives Processed: %zu (%.0f%%)", m.primitives_processed,
              m.primitives_processed * 100.f / m.primitive_count);
  ImGui::Text("Back Faces Culled: %zu (%.0f%%)", m.face_culling,
              m.face_culling * 100.f / m.primitive_count);
  ImGui::Text("Empty Primitive: %zu (%.0f%%)", m.empty_primitive,
              m.empty_primitive * 100.f / m.primitive_count);
  ImGui::Text("Scissor Culled: %zu (%.0f%%)", m.scissor_culling,
              m.scissor_culling * 100.f / m.primitive_count);
  ImGui::Text("Sample Point Culled: %zu (%.0f%%)", m.sample_point_culling,
              m.sample_point_culling * 100.f / m.primitive_count);
  ImGui::Text("Early Fragment Checks Tripped: %zu", m.early_fragment_test);
  ImGui::Text("Vertex Invocations: %zu", m.vertex_invocations);
  ImGui::Text(
      "Fragment Invocations: %zu (%.2fx screen)", m.fragment_invocations,
      static_cast<ScalarF>(m.fragment_invocations) / (m.area.x * m.area.y));

  ImGui::End();
}

bool Application::OnRender() {
  if (!is_valid_) {
    return false;
  }

  const auto update_start = Clock::now();

  ImGui_ImplSFT_NewFrame();
  ImGui::NewFrame();

  rasterizer_->ResetMetrics();

  if (!Update()) {
    return false;
  }

  DisplayMetrics(rasterizer_->GetMetrics());
  rasterizer_->ResetMetrics();

  ImGui::Render();

  ImGui_ImplSFT_RenderDrawData(rasterizer_.get(), ImGui::GetDrawData());

  rasterizer_->Finish();

  const auto size = rasterizer_->GetSize();

  auto& pass = rasterizer_->GetRenderPass();
  std::shared_ptr<Texture<Color>> texture;
  if (pass.color.texture->GetSampleCount() == SampleCount::kOne) {
    texture = pass.color.texture;
  } else {
    if (!pass.color.texture->Resolve(*pass.color.resolve)) {
      return false;
    }
    texture = pass.color.resolve;
  }

  last_update_duration_ = Clock::now() - update_start;

  SDLTextureNoCopyCaster color_attachment(sdl_renderer_,               //
                                          texture->Get({}, 0),         //
                                          size.x,                      //
                                          size.y,                      //
                                          texture->GetBytesPerPixel()  //
  );

  SDL_Rect dest = {};
  dest.x = 0;
  dest.y = 0;
  dest.w = size.x;
  dest.h = size.y;
  {
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
  }

  ::SDL_RenderPresent(sdl_renderer_);
  return true;
}

SecondsF Application::GetTimeSinceLaunch() const {
  return Clock::now() - launch_time_;
}

bool Application::OnWindowSizeChanged(glm::ivec2 size) {
  if (!rasterizer_) {
    return false;
  }
  return rasterizer_->Resize(size);
}

void Application::SetTitle(std::string title) {
  title_ = std::move(title);
}

void Application::SetRasterizerCallback(RasterizerCallback callback) {
  rasterizer_callback_ = callback;
}

}  // namespace sft
