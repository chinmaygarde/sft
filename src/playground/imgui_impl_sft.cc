/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "imgui_impl_sft.h"

#include "backends/imgui_impl_sdl.h"
#include "image.h"
#include "imgui_shader.h"
#include "macros.h"
#include "rasterizer.h"

namespace sft {

struct RendererData {
  std::shared_ptr<Image> font_atlas;
  std::shared_ptr<ImGuiShader> shader;
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
  auto font_atlas =
      Image::Create(Mapping::MakeWithCopy(pixels, width * height * 4),
                    glm::ivec2{width, height});
  font_atlas->SetSampler({.min_mag_filter = Filter::kNearest});

  io.Fonts->SetTexID(font_atlas.get());

  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.offset =
      offsetof(ImGuiShader::VertexData, vertex_position);
  pipeline->vertex_descriptor.stride = sizeof(ImGuiShader::VertexData);
  pipeline->vertex_descriptor.vertex_format = VertexFormat::kFloat2;

  data->font_atlas = font_atlas;
  data->shader = shader;
  data->pipeline = pipeline;

  return true;
}

void ImGui_ImplSFT_NewFrame() {
  ImGui_ImplSDL2_NewFrame();
}

ImGuiShader::VertexData ToShaderVertexData(const ImDrawVert& v) {
  ImGuiShader::VertexData result;
  result.vertex_color = Color{v.col};
  result.vertex_position = {v.pos.x, v.pos.y};
  result.texture_coordinates = {v.uv.x, v.uv.y};
  return result;
}

constexpr Rect ToScissorRect(const ImVec2& display_size,
                             const ImVec4& clip,
                             const ImVec2& scale) {
  return Rect::MakeLTRB(clip.x * scale.x,                     //
                        (display_size.y - clip.y) * scale.y,  //
                        clip.z * scale.x,                     //
                        (display_size.y - clip.w) * scale.y   //
  );
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

  auto vertex_buffer = Buffer::Create();
  auto uniform_buffer = Buffer::Create();

  SFT_ASSERT(draw->DisplayPos.x == 0 && draw->DisplayPos.y == 0);

  uniform_buffer->Emplace<ImGuiShader::Uniforms>(
      {.mvp = glm::ortho<ScalarF>(0,                    // left
                                  draw->DisplaySize.x,  // right
                                  draw->DisplaySize.y,  // bottom
                                  0                     // top
                                  )});

  for (int i = 0; i < draw->CmdListsCount; i++) {
    const ImDrawList* draw_list = draw->CmdLists[i];
    const auto& cmd_buffer = draw_list->CmdBuffer;
    const ImVector<ImDrawVert>& vtx_buffer = draw_list->VtxBuffer;
    const ImVector<ImDrawIdx>& idx_buffer = draw_list->IdxBuffer;
    for (const auto& cmd : cmd_buffer) {
      if (cmd.UserCallback) {
        cmd.UserCallback(draw_list, &cmd);
        continue;
      }
      SFT_ASSERT(cmd.VtxOffset == 0);
      SFT_ASSERT(cmd.ElemCount % 3 == 0);

      user_data->pipeline->scissor = ToScissorRect(
          draw->DisplaySize, cmd.ClipRect, io.DisplayFramebufferScale);

      size_t index_offset = cmd.IdxOffset;

      const auto vtx_buffer_start = vertex_buffer->GetLength();

      for (size_t e = 0; e < cmd.ElemCount; e += 3) {
        const auto v1_idx = idx_buffer[index_offset++];
        const auto v2_idx = idx_buffer[index_offset++];
        const auto v3_idx = idx_buffer[index_offset++];

        const auto v1 = ToShaderVertexData(vtx_buffer[v1_idx]);
        const auto v2 = ToShaderVertexData(vtx_buffer[v2_idx]);
        const auto v3 = ToShaderVertexData(vtx_buffer[v3_idx]);

        vertex_buffer->Emplace(v1);
        vertex_buffer->Emplace(v2);
        vertex_buffer->Emplace(v3);
      }

      const auto vtx_buffer_end = vertex_buffer->GetLength();

      auto texture = reinterpret_cast<Image*>(cmd.GetTexID());

      sft::Uniforms uniforms;
      uniforms.buffer = *uniform_buffer;
      uniforms.images[0u] = texture->shared_from_this();

      rasterizer->Draw(
          user_data->pipeline,  // pipeline
          BufferView{*vertex_buffer, vtx_buffer_start,
                     vtx_buffer_end - vtx_buffer_start},  // vertex_buffer
          uniforms,                                       // uniform buffer
          cmd.ElemCount                                   // count
      );
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
