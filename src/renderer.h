#pragma once

#include <SDL.h>
#include <memory>
#include "geom.h"
#include "image.h"
#include "model.h"
#include "sdl_utils.h"

namespace sft {

class Renderer {
 public:
  Renderer();

  ~Renderer();

  bool Render();

  bool Update();

 private:
  std::unique_ptr<Image> image_;
  glm::ivec2 render_surface_size_;
  glm::ivec2 window_size_;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<Model> model_;
  bool is_valid_ = false;

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
};

}  // namespace sft
