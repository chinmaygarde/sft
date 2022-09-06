#include <SDL.h>
#include <iostream>
#include <memory>

#include "demo_application.h"
#include "geom.h"
#include "rasterizer.h"

namespace sft {

bool Main(int argc, char const* argv[]) {
  if (::SDL_Init(SDL_INIT_VIDEO) != 0) {
    return false;
  }

  bool is_running = true;

  RasterizerApplication application;
  while (is_running) {
    is_running = application.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          if (event.button.button != SDL_BUTTON_LEFT) {
            break;
          }
          application.OnTouchEvent(event.type == SDL_MOUSEBUTTONDOWN
                                       ? TouchEventType::kTouchDown
                                       : TouchEventType::kTouchUp,
                                   glm::vec2{event.button.x, event.button.y});
          break;
        case SDL_MOUSEMOTION:
          if (event.button.button != SDL_BUTTON_LEFT) {
            break;
          }
          application.OnTouchEvent(TouchEventType::kTouchMove,
                                   glm::vec2{event.button.x, event.button.y});
          break;
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDL_KeyCode::SDLK_q:
            case SDL_KeyCode::SDLK_ESCAPE:
              is_running = false;
              break;
            default:
              break;
          }
          break;
        case SDL_QUIT:
          is_running = false;
          break;
      }
    }
  }

  ::SDL_Quit();

  return true;
}

}  // namespace sft

int main(int argc, char const* argv[]) {
  return sft::Main(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
