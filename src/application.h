#pragma once

#include <SDL.h>

#include <memory>

#include "event.h"
#include "geom.h"
#include "macros.h"
#include "renderer.h"
#include "sdl_utils.h"

namespace sft {

class Application {
 public:
  virtual ~Application();

  bool IsValid() const;

  bool Render();

  virtual bool Update();

  virtual void OnTouchEvent(TouchEventType type, glm::vec2 pos);

 protected:
  Application(std::shared_ptr<Renderer> renderer);

  const std::shared_ptr<Renderer> renderer_;

 private:
  glm::ivec2 window_size_;
  SDL_Window* sdl_window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  bool is_valid_ = false;

  SFT_DISALLOW_COPY_AND_ASSIGN(Application);
};

}  // namespace sft
