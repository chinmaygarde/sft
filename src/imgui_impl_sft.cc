#include "imgui_impl_sft.h"

#include "backends/imgui_impl_sdl.h"
#include "macros.h"
#include "texture.h"

namespace sft {

struct RendererData {
  std::shared_ptr<Texture> font_atlas_;
};

bool ImGui_ImplSFT_Init(SDL_Window* window, SDL_Renderer* renderer) {
  auto result = ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  if (!result) {
    return false;
  }

  auto& io = ImGui::GetIO();
  SFT_ASSERT(io.BackendRendererUserData == nullptr);
  SFT_ASSERT(io.BackendRendererName == nullptr);
  auto data = new RendererData{};
  io.BackendRendererUserData = data;
  io.BackendRendererName = "SFT";
  uint8_t* pixels = nullptr;
  int width = 0;
  int height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
  data->font_atlas_ = std::make_shared<Texture>(
      Mapping::MakeWithCopy(pixels, width * height * 4),
      glm::ivec2{width, height});

  return true;
}

void ImGui_ImplSFT_NewFrame() {
  ImGui_ImplSDL2_NewFrame();
}

void ImGui_ImplSFT_RenderDrawData(ImDrawData* draw) {
  if (!draw || draw->CmdListsCount <= 0) {
    // Nothing to draw.
    return;
  }
}

void ImGui_ImplSFT_Shutdown() {
  auto& io = ImGui::GetIO();
  SFT_ASSERT(io.BackendRendererUserData != nullptr);
  SFT_ASSERT(io.BackendRendererName != nullptr);
  delete reinterpret_cast<RendererData*>(io.BackendRendererUserData);
  io.BackendRendererUserData = nullptr;
  io.BackendRendererName = nullptr;
  ImGui_ImplSDL2_Shutdown();
}
}  // namespace sft
