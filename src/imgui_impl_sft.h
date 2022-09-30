#pragma once

#include <SDL.h>
#include "imgui.h"

namespace sft {

class Rasterizer;

bool ImGui_ImplSFT_Init(SDL_Window* window, SDL_Renderer* renderer);

void ImGui_ImplSFT_NewFrame();

void ImGui_ImplSFT_RenderDrawData(Rasterizer* rasterizer,
                                  ImDrawData* draw_data);

void ImGui_ImplSFT_Shutdown();

}  // namespace sft
