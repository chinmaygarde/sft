#pragma once

#include <SDL.h>

#include <memory>

#include "event.h"
#include "geom.h"
#include "macros.h"
#include "renderer.h"
#include "sdl_utils.h"
#include "timing.h"

namespace sft {

class Rasterizer;

class Application {
 public:
  virtual ~Application();

  bool IsValid() const;

  bool Render();

  virtual bool Update();

  virtual void OnTouchEvent(TouchEventType type, glm::vec2 pos);

  virtual bool OnWindowSizeChanged(glm::ivec2 size);

  SecondsF GetTimeSinceLaunch() const;

  virtual Rasterizer* GetHUDRasterizer() const;

 protected:
  Application(std::shared_ptr<Renderer> renderer);

  const std::shared_ptr<Renderer> renderer_;

 private:
  glm::ivec2 window_size_;
  SDL_Window* sdl_window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  std::chrono::time_point<Clock> launch_time_;
  std::chrono::time_point<Clock> last_title_update_;
  std::chrono::nanoseconds last_update_duration_;
  bool is_valid_ = false;

  bool OnRender();

  SFT_DISALLOW_COPY_AND_ASSIGN(Application);
};

}  // namespace sft
