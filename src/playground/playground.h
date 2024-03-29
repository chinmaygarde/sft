/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <SDL.h>

#include <memory>

#include "geometry.h"
#include "macros.h"
#include "marl/scheduler.h"
#include "sdl_utils.h"
#include "texture.h"
#include "timing.h"

namespace sft {

class Rasterizer;

class Playground {
 public:
  Playground(glm::ivec2 size = {800, 600},
             SampleCount sample_count = SampleCount::kOne);

  ~Playground();

  bool IsValid() const;

  bool Render();

  void SetTitle(std::string title);

  bool Update();

  bool OnWindowSizeChanged(glm::ivec2 size);

  SecondsF GetTimeSinceLaunch() const;

  using RasterizerCallback = std::function<bool(Rasterizer&)>;

  void SetRasterizerCallback(RasterizerCallback callback);

 private:
  std::unique_ptr<marl::Scheduler> scheduler_;
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

  SFT_DISALLOW_COPY_AND_ASSIGN(Playground);
};

}  // namespace sft
