#include "runner.h"

#include <SDL.h>

#include "application.h"

namespace sft {

Runner::Runner() {
  SFT_ASSERT(::SDL_Init(SDL_INIT_VIDEO) == 0);
}

bool Runner::Run(Application& application) const {
  bool is_running = true;
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
  return true;
}

Runner::~Runner() {
  ::SDL_Quit();
}

}  // namespace sft
