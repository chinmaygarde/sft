#pragma once

#include <SDL.h>

#include <memory>

#include "event.h"
#include "framebuffer.h"
#include "geom.h"
#include "macros.h"
#include "sdl_utils.h"
#include "timing.h"

namespace sft {

class Rasterizer;

class Application {
 public:
  Application(glm::ivec2 size = {1024, 768},
              SampleCount sample_count = SampleCount::kOne);

  ~Application();

  bool IsValid() const;

  bool Render();

  void SetTitle(std::string title);

  bool Update();

  void OnTouchEvent(TouchEventType type, glm::vec2 pos);

  bool OnWindowSizeChanged(glm::ivec2 size);

  SecondsF GetTimeSinceLaunch() const;

  Rasterizer* GetHUDRasterizer() const;

  using RasterizerCallback = std::function<bool(Rasterizer&)>;

  void SetRasterizerCallback(RasterizerCallback callback);

 private:
  const std::shared_ptr<Rasterizer> rasterizer_;
  glm::ivec2 window_size_;
  SDL_Window* sdl_window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  std::chrono::time_point<Clock> launch_time_;
  std::chrono::time_point<Clock> last_title_update_;
  std::chrono::nanoseconds last_update_duration_;
  std::string title_;
  RasterizerCallback rasterizer_callback_;
  bool is_valid_ = false;

  bool OnRender();

  SFT_DISALLOW_COPY_AND_ASSIGN(Application);
};

}  // namespace sft
