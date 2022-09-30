#include "imgui_impl_sft.h"

#include "backends/imgui_impl_sdl.h"
#include "macros.h"
#include "shaders/imgui_shader.h"
#include "texture.h"

namespace sft {

struct RendererData {
  std::shared_ptr<Pipeline> pipeline;
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
  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<ImGuiShader>();
  auto font_atlas = std::make_shared<Texture>(
      Mapping::MakeWithCopy(pixels, width * height * 4),
      glm::ivec2{width, height});

  shader->SetTexture(font_atlas);

  pipeline->shader = shader;
  pipeline->blend_mode = BlendMode::kSourceOver;
  pipeline->vertex_descriptor.offset =
      offsetof(ImGuiShader::VertexData, vertex_position);
  pipeline->vertex_descriptor.stride = sizeof(ImGuiShader::VertexData);

  data->pipeline = pipeline;

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
