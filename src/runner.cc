#include "runner.h"

#include <SDL.h>

#include "application.h"
#include "backends/imgui_impl_sdl.h"
#include "imgui.h"

namespace sft {

Runner::Runner() : start_time_(Clock::now()) {
  SFT_ASSERT(::SDL_Init(SDL_INIT_VIDEO) == 0);
}

SecondsF Runner::ElapsedTime() const {
  return Clock::now() - start_time_;
}

bool Runner::Run(Application& application) const {
  bool is_running = true;
  bool success = true;
  while (is_running) {
    success = is_running = application.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      auto& io = ImGui::GetIO();

      ImGui_ImplSDL2_ProcessEvent(&event);

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
  return success;
}

Runner::~Runner() {
  ::SDL_Quit();
}

}  // namespace sft
