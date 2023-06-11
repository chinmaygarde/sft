/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "playground_test.h"

#include <SDL.h>

#include <sstream>

#include "backends/imgui_impl_sdl.h"
#include "fixtures_location.h"
#include "imgui.h"
#include "playground.h"

namespace sft {

PlaygroundTest::PlaygroundTest() : start_time_(Clock::now()) {
  SFT_ASSERT(::SDL_Init(SDL_INIT_VIDEO) == 0);
}

SecondsF PlaygroundTest::ElapsedTime() const {
  return Clock::now() - start_time_;
}

static std::string CreateTestName() {
  std::stringstream stream;
  stream << ::testing::UnitTest::GetInstance()
                ->current_test_info()
                ->test_suite_name();
  stream << ".";
  stream << ::testing::UnitTest::GetInstance()->current_test_info()->name();
  return stream.str();
}

static bool gSkipRemainingTests = false;

bool PlaygroundTest::Run(Playground& playground) const {
  bool is_running = true;
  bool success = true;
  playground.SetTitle(CreateTestName());
  while (is_running) {
    success = is_running = playground.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      auto& io = ImGui::GetIO();

      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDL_KeyCode::SDLK_q:
            case SDL_KeyCode::SDLK_ESCAPE:
              if ((event.key.keysym.mod & KMOD_LSHIFT) ||
                  (event.key.keysym.mod & KMOD_RSHIFT) ||
                  (event.key.keysym.mod & KMOD_LCTRL) ||
                  (event.key.keysym.mod & KMOD_RCTRL)) {
                gSkipRemainingTests = true;
              }
              is_running = false;
              break;
            default:
              break;
          }
          break;
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              if (!playground.OnWindowSizeChanged(
                      {event.window.data1, event.window.data2})) {
                std::cout << "Window resizing failed." << std::endl;
                is_running = false;
              }

              break;
          }
          break;
      }
    }
  }
  return success;
}

PlaygroundTest::~PlaygroundTest() {
  ::SDL_Quit();
}

void PlaygroundTest::SetUp() {
  if (gSkipRemainingTests) {
    GTEST_SKIP();
  }
}

void PlaygroundTest::TearDown() {}

}  // namespace sft
