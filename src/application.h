#pragma once

#include <SDL.h>
#include <memory>
#include "event.h"
#include "geom.h"
#include "model.h"
#include "pipeline.h"
#include "rasterizer.h"
#include "sdl_utils.h"

namespace sft {

class Application {
 public:
  Application();

  ~Application();

  bool Render();

  bool Update();

  void OnTouchEvent(TouchEventType type, glm::vec2 pos);

 private:
  std::unique_ptr<Rasterizer> rasterizer_;
  glm::ivec2 window_size_;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<Model> model_;
  glm::vec2 touch_offset_;
  std::optional<glm::vec2> last_touch_;
  std::shared_ptr<Pipeline> pipeline_;
  bool is_valid_ = false;

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
};

}  // namespace sft
