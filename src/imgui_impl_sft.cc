#include "imgui_impl_sft.h"

#include "backends/imgui_impl_sdl.h"
#include "macros.h"
#include "rasterizer.h"
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

  pipeline->depth_test_enabled = false;
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

ImGuiShader::VertexData ToShaderVertexData(const ImDrawVert& v) {
  ImGuiShader::VertexData result;
  result.vertex_color = Color{v.col};
  result.vertex_position = {v.pos.x, v.pos.y, 0.0};
  result.texture_coordinates = {v.uv.x, v.uv.y};
  return result;
}

void ImGui_ImplSFT_RenderDrawData(Rasterizer* rasterizer, ImDrawData* draw) {
  if (!draw || draw->CmdListsCount <= 0) {
    // Nothing to draw.
    return;
  }

  auto vtx_bytes = draw->TotalVtxCount * sizeof(ImDrawVert);
  auto idx_bytes = draw->TotalIdxCount * sizeof(ImDrawIdx);

  if (vtx_bytes == 0 || idx_bytes == 0) {
    // Nothing to draw.
    return;
  }

  auto& io = ImGui::GetIO();
  SFT_ASSERT(io.BackendRendererUserData != nullptr);
  auto user_data = reinterpret_cast<RendererData*>(io.BackendRendererUserData);

  Buffer uniform_buffer;

  uniform_buffer.Emplace<ImGuiShader::Uniforms>(
      {.mvp = glm::ortho<ScalarF>(0,                    // left
                                  draw->DisplaySize.x,  // right
                                  0,                    // bottom
                                  draw->DisplaySize.y   // top
                                  )});

  for (int i = 0; i < draw->CmdListsCount; i++) {
    const ImDrawList* draw_list = draw->CmdLists[i];
    const auto& cmd_buffer = draw_list->CmdBuffer;
    const ImVector<ImDrawVert>& vtx_buffer = draw_list->VtxBuffer;
    const ImVector<ImDrawIdx>& idx_buffer = draw_list->IdxBuffer;
    for (const auto& cmd : cmd_buffer) {
      SFT_ASSERT(cmd.VtxOffset == 0);
      SFT_ASSERT(cmd.ElemCount % 3 == 0);

      size_t index_offset = cmd.IdxOffset;

      for (size_t e = 0; e < cmd.ElemCount; e += 3) {
        const auto v1_idx = idx_buffer[index_offset++];
        const auto v2_idx = idx_buffer[index_offset++];
        const auto v3_idx = idx_buffer[index_offset++];

        const auto v1 = ToShaderVertexData(vtx_buffer[v1_idx]);
        const auto v2 = ToShaderVertexData(vtx_buffer[v2_idx]);
        const auto v3 = ToShaderVertexData(vtx_buffer[v3_idx]);

        // Inefficient but we don't have buffer views.
        Buffer vertex_buffer;
        vertex_buffer.Emplace(v1);
        vertex_buffer.Emplace(v2);
        vertex_buffer.Emplace(v3);

        rasterizer->Draw(*user_data->pipeline,  // pipeline
                         vertex_buffer,         // vertex_buffer
                         uniform_buffer,        // uniform buffer
                         3u                     // count
        );
      }
    }
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
