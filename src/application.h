#pragma once

#include <SDL.h>

#include <memory>

#include "event.h"
#include "geom.h"
#include "macros.h"
#include "rasterizer.h"
#include "sdl_utils.h"

namespace sft {

class Application {
 public:
  Application();

  ~Application();

  bool IsValid() const;

  bool Render();

  glm::ivec2 GetRenderSurfaceSize() const;

  virtual bool Update() = 0;

  virtual void OnTouchEvent(TouchEventType type, glm::vec2 pos);

 protected:
  std::unique_ptr<Rasterizer> rasterizer_;

 private:
  glm::ivec2 render_surface_size_ = {800, 600};
  glm::ivec2 window_size_;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool is_valid_ = false;

  SFT_DISALLOW_COPY_AND_ASSIGN(Application);
};

}  // namespace sft
