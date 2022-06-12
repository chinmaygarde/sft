#include <SDL.h>
#include <iostream>
#include <memory>

#include "geom.h"
#include "image.h"
#include "renderer.h"

namespace sft {

bool Main(int argc, char const* argv[]) {
  if (::SDL_Init(SDL_INIT_VIDEO) != 0) {
    return false;
  }

  bool is_running = true;

  Renderer renderer;
  while (is_running) {
    is_running = renderer.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      switch (event.type) {
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
