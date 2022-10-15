#include <gtest/gtest.h>

#include "application.h"
#include "buffer.h"
#include "canvas.h"
#include "fixtures_location.h"
#include "imgui.h"
#include "model.h"
#include "pipeline.h"
#include "rasterizer.h"
#include "shaders/color_shader.h"
#include "shaders/texture_shader.h"
#include "test_runner.h"

namespace sft {
namespace testing {

using RayTracerTest = TestRunner;
using RasterizerTest = TestRunner;

TEST_F(RasterizerTest, CanClearRasterizer) {
  Application application;
  application.SetRasterizerCallback([](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFuchsia);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawTexturedImage) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.7, 0.5, 0.0};
  glm::vec3 p2 = {0.7, 0.5, 0.0};
  glm::vec3 p3 = {0.7, -0.5, 0.0};
  glm::vec3 p4 = {-0.7, -0.5, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {1.0, 0.0};
  glm::vec2 br = {1.0, 1.0};
  glm::vec2 bl = {0.0, 1.0};
  auto vertex_buffer = Buffer::Create();
  vertex_buffer->Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });

  auto texture1 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  auto texture2 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "boston.jpg");
  auto texture3 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "kalimba.jpg");
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    {
      auto uniform_buffer = Buffer::Create();
      uniform_buffer->Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0, 0},
      });
      shader->SetTexture(texture1);
      rasterizer.Draw(*pipeline, *vertex_buffer, *uniform_buffer, 6);
    }
    {
      auto uniform_buffer = Buffer::Create();
      uniform_buffer->Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0.2, 0.2},
      });
      shader->SetTexture(texture2);
      rasterizer.Draw(*pipeline, *vertex_buffer, *uniform_buffer, 6);
    }
    {
      auto uniform_buffer = Buffer::Create();
      uniform_buffer->Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0.4, 0.4},
      });
      shader->SetTexture(texture3);
      rasterizer.Draw(*pipeline, *vertex_buffer, *uniform_buffer, 6);
    }
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawWithIndexBuffer16Bit) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.7, 0.5, 0.0};
  glm::vec3 p2 = {0.7, 0.5, 0.0};
  glm::vec3 p3 = {0.7, -0.5, 0.0};
  glm::vec3 p4 = {-0.7, -0.5, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {1.0, 0.0};
  glm::vec2 br = {1.0, 1.0};
  glm::vec2 bl = {0.0, 1.0};

  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      {tl, p1},  // 0
      {tr, p2},  // 1
      {br, p3},  // 2
      {bl, p4},  // 3
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .alpha = 1.0,
      .offset = {0, 0},
  });
  auto index_buffer = buffer->Emplace(std::vector<uint16_t>{0, 1, 2, 2, 3, 0});
  auto texture1 =
      std::make_shared<Texture>(SFT_ASSETS_LOCATION "embarcadero.jpg");
  shader->SetTexture(texture1);
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.index_type = IndexType::kUInt16;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    rasterizer.Draw(*pipeline, vertex_buffer, index_buffer, uniform_buffer, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

// Compare with the outputs of
// https://www.w3.org/TR/compositing-1/#porterduffcompositingoperators
TEST_F(RasterizerTest, CanBlendWithDifferentModes) {
  Application application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<ColorShader>();

  auto buffer = Buffer::Create();

  auto index_buffer = buffer->Emplace(std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
  auto src_vertex = buffer->Emplace(std::vector<VD>{
      {{-0.25, 0.25, 0.0}},
      {{1, 0.25, 0.0}},
      {{1, -1, 0.0}},
      {{-0.25, -1, 0.0}},
  });
  auto dst_vertex = buffer->Emplace(std::vector<VD>{
      {{-1, 1, 0.0}},
      {{0.25, 1.0, 0.0}},
      {{0.25, -0.25, 0.0}},
      {{-1, -0.25, 0.0}},
  });
  auto src_uniform = buffer->Emplace(Uniforms{
      .color = kColorSkyBlue,
  });
  auto dst_uniform = buffer->Emplace(Uniforms{
      .color = kColorYellow,
  });
  pipeline->shader = shader;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorWhite);
    const char* items[] = {
        "Clear",           "Copy",       "Destination",     "SourceOver",
        "DestinationOver", "SourceIn",   "DestinationIn",   "SourceOut",
        "DestinationOut",  "SourceAtop", "DestinationAtop", "XOR",
    };
    static int selected = static_cast<int>(BlendMode::kSourceOver);
    ImGui::Combo("Blend Mode", &selected, items,
                 sizeof(items) / sizeof(const char*));
    ImGui::TextWrapped(
        "Compare outputs to "
        "https://www.w3.org/TR/compositing-1/#porterduffcompositingoperators");
    pipeline->color_desc.blend =
        BlendDescriptorForMode(static_cast<BlendMode>(selected));
    rasterizer.Draw(*pipeline, dst_vertex, index_buffer, dst_uniform, 6);
    rasterizer.Draw(*pipeline, src_vertex, index_buffer, src_uniform, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawWithIndexBuffer32Bit) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.7, 0.5, 0.0};
  glm::vec3 p2 = {0.7, 0.5, 0.0};
  glm::vec3 p3 = {0.7, -0.5, 0.0};
  glm::vec3 p4 = {-0.7, -0.5, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {1.0, 0.0};
  glm::vec2 br = {1.0, 1.0};
  glm::vec2 bl = {0.0, 1.0};

  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      {tl, p1},  // 0
      {tr, p2},  // 1
      {br, p3},  // 2
      {bl, p4},  // 3
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .alpha = 1.0,
      .offset = {0, 0},
  });
  auto index_buffer = buffer->Emplace(std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
  auto texture1 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  shader->SetTexture(texture1);
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.index_type = IndexType::kUInt32;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    rasterizer.Draw(*pipeline, vertex_buffer, index_buffer, uniform_buffer, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanCompareLinearAndNearestSampling) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.4, 0.4, 0.0};
  glm::vec3 p2 = {0.4, 0.4, 0.0};
  glm::vec3 p3 = {0.4, -0.4, 0.0};
  glm::vec3 p4 = {-0.4, -0.4, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {1.0, 0.0};
  glm::vec2 br = {1.0, 1.0};
  glm::vec2 bl = {0.0, 1.0};
  auto vertex_buffer = Buffer::Create();
  vertex_buffer->Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });

  auto texture1 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  texture1->SetSampler({.min_mag_filter = Filter::kLinear});
  auto texture2 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  texture2->SetSampler({.min_mag_filter = Filter::kNearest});
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    {
      auto uniform_buffer = Buffer::Create();
      uniform_buffer->Emplace(Uniforms{
          .alpha = 1.0,
          .offset = {0, -0.5},
      });
      shader->SetTexture(texture1);
      rasterizer.Draw(*pipeline, *vertex_buffer, *uniform_buffer, 6);
    }
    {
      auto uniform_buffer = Buffer::Create();
      uniform_buffer->Emplace(Uniforms{
          .alpha = 1.0,
          .offset = {0, 0.5},
      });
      shader->SetTexture(texture2);
      rasterizer.Draw(*pipeline, *vertex_buffer, *uniform_buffer, 6);
    }

    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanWrapModeRepeatAndMirror) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.7, 0.5, 0.0};
  glm::vec3 p2 = {0.7, 0.5, 0.0};
  glm::vec3 p3 = {0.7, -0.5, 0.0};
  glm::vec3 p4 = {-0.7, -0.5, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {6.0, 0.0};
  glm::vec2 br = {6.0, 6.0};
  glm::vec2 bl = {0.0, 6.0};
  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .alpha = 1.0,
      .offset = {0, 0},
  });
  auto texture1 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  Sampler sampler;
  sampler.wrap_mode_s = WrapMode::kRepeat;
  sampler.wrap_mode_t = WrapMode::kMirror;
  texture1->SetSampler(sampler);
  shader->SetTexture(texture1);
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanWrapModeClampAndRepeat) {
  Application application;

  using VD = TextureShader::VertexData;
  using Uniforms = TextureShader::Uniforms;

  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();

  glm::vec3 p1 = {-0.7, 0.5, 0.0};
  glm::vec3 p2 = {0.7, 0.5, 0.0};
  glm::vec3 p3 = {0.7, -0.5, 0.0};
  glm::vec3 p4 = {-0.7, -0.5, 0.0};

  glm::vec2 tl = {0.0, 0.0};
  glm::vec2 tr = {6.0, 0.0};
  glm::vec2 br = {6.0, 6.0};
  glm::vec2 bl = {0.0, 6.0};
  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .alpha = 1.0,
      .offset = {0, 0},
  });
  auto texture1 = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  Sampler sampler;
  sampler.wrap_mode_s = WrapMode::kClamp;
  sampler.wrap_mode_t = WrapMode::kMirror;
  texture1->SetSampler(sampler);
  shader->SetTexture(texture1);
  pipeline->shader = shader;
  pipeline->color_desc.blend.enabled = true;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawTeapot) {
  Application application;
  Model model(SFT_ASSETS_LOCATION "teapot/teapot.obj",
              SFT_ASSETS_LOCATION "teapot");
  model.SetScale(0.075);
  auto texture = std::make_shared<Texture>(SFT_ASSETS_LOCATION "marble.jpg");
  texture->SetSampler({.min_mag_filter = Filter::kLinear});
  model.SetTexture(texture);
  ASSERT_TRUE(model.IsValid());
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    model.SetRotation(application.GetTimeSinceLaunch().count() * 45);
    model.RenderTo(rasterizer);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

static void DisplayTextureInHUD(const char* title,
                                Texture* tex,
                                ScalarF scale) {
  if (!tex) {
    return;
  }
  ImGui::Text(title, "");
  const auto tint_color = ImVec4(1, 1, 1, 1);
  const auto border_color = ImVec4(1, 1, 1, 1);
  const auto tex_size = tex->GetSize();
  const auto image_pos = ImGui::GetCursorScreenPos();
  const auto image_size = glm::vec2{tex_size.x * scale, tex_size.y * scale};
  ImGui::Image(tex,                                 //
               ImVec2(image_size.x, image_size.y),  //
               ImVec2(0, 1),                        // uv0
               ImVec2(1, 0),                        // uv1
               tint_color,                          // tint color
               border_color                         // border color
  );
  auto& io = ImGui::GetIO();
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    constexpr auto min_uv = glm::vec2{0.0f, 0.0f};
    constexpr auto max_uv = glm::vec2{1.0f, 1.0f};
    auto uv = glm::clamp(
        glm::vec2{io.MousePos.x - image_pos.x, io.MousePos.y - image_pos.y} /
            image_size,
        min_uv, max_uv);
    constexpr auto zoom_factor = 40.0f;
    auto zoom_uv_correction = glm::vec2{1.f / zoom_factor};
    auto uv1 = glm::clamp(uv - zoom_uv_correction, min_uv, max_uv);
    auto uv2 = glm::clamp(uv + zoom_uv_correction, min_uv, max_uv);
    uv.y = 1.f - uv.y;
    uv1.y = 1.f - uv1.y;
    uv2.y = 1.f - uv2.y;
    const auto tooltip_image_size = image_size / 2.0f;
    auto sampled = tex->Sample(uv);
    ImGui::ColorEdit4("Color", (float*)&sampled);
    ImGui::Text("%s (%.2fx)", title, zoom_factor);
    ImGui::Image(tex,                                                 // texture
                 ImVec2(tooltip_image_size.x, tooltip_image_size.y),  // size
                 ImVec2{uv1.x, uv1.y},                                // uv1
                 ImVec2{uv2.x, uv2.y},                                // uv2
                 tint_color,   // tint color
                 border_color  // border color
    );
    ImGui::EndTooltip();
  }
}

TEST_F(RasterizerTest, CanDrawHelmet) {
  Application application;
  Model model(SFT_ASSETS_LOCATION "helmet/Helmet.obj",
              SFT_ASSETS_LOCATION "helmet");

  auto texture =
      std::make_shared<Texture>(SFT_ASSETS_LOCATION "helmet/Base.png");
  texture->SetSampler({.min_mag_filter = Filter::kNearest});
  model.SetTexture(texture);
  model.GetPipeline().stencil_desc = StencilAttachmentDescriptor{
      .stencil_test_enabled = true,
      .stencil_compare = CompareFunction::kAlways,
      .depth_stencil_pass = StencilOperation::kIncrementClamp,
  };
  ASSERT_TRUE(model.IsValid());
  static std::shared_ptr<Texture> depth_tex;
  static std::shared_ptr<Texture> overdraw_tex;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    static float rotation = 45 + 90;
    static float scale = 4;
    ImGui::SliderFloat("Rotation", &rotation, 0, 360);
    ImGui::SliderFloat("Scale", &scale, 0.1f, 25.0f);
    model.SetRotation(rotation);
    model.SetScale(scale);
    model.RenderTo(rasterizer);
    depth_tex = rasterizer.CaptureDebugDepthTexture();
    overdraw_tex = rasterizer.CaptureDebugStencilTexture();
    DisplayTextureInHUD("Depth Buffer", depth_tex.get(), 0.25);
    DisplayTextureInHUD("Overdraw", overdraw_tex.get(), 0.25);
    return true;
  });
  ASSERT_TRUE(Run(application));
  depth_tex = nullptr;
  overdraw_tex = nullptr;
}

TEST_F(RasterizerTest, CanMSAA) {
  Application application({1024, 768}, SampleCount::kFour);
  Model model(SFT_ASSETS_LOCATION "teapot/teapot.obj",
              SFT_ASSETS_LOCATION "teapot");
  model.SetScale(0.075);
  auto texture = std::make_shared<Texture>(SFT_ASSETS_LOCATION "marble.jpg");
  texture->SetSampler({.min_mag_filter = Filter::kLinear});
  model.SetTexture(texture);
  ASSERT_TRUE(model.IsValid());
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    model.SetRotation(application.GetTimeSinceLaunch().count() * 45);
    model.RenderTo(rasterizer);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanCullFaces) {
  Application application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();
  pipeline.vertex_descriptor.offset = offsetof(VD, position);
  pipeline.vertex_descriptor.stride = sizeof(VD);
  // This should be culled and nothing will show up.
  pipeline.cull_face = CullFace::kBack;
  pipeline.winding = Winding::kCounterClockwise;
  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, 0.0}},
      VD{.position = {0.0, 1.0, 0.0}},
      VD{.position = {1.0, -1.0, 0.0}},
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .color = kColorFirebrick,
  });
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    rasterizer.Draw(pipeline, vertex_buffer, uniform_buffer, 3u);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanApplyScissor) {
  Application application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();
  pipeline.vertex_descriptor.offset = offsetof(VD, position);
  pipeline.vertex_descriptor.stride = sizeof(VD);
  auto buffer = Buffer::Create();
  auto vertex_buffer = buffer->Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, 0.0}},
      VD{.position = {0.0, 1.0, 0.0}},
      VD{.position = {1.0, -1.0, 0.0}},
  });
  auto uniform_buffer = buffer->Emplace(Uniforms{
      .color = kColorFirebrick,
  });
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    const auto sz = rasterizer.GetSize();
    pipeline.scissor = Rect{sz.x / 2.0f, 0, sz.x / 2.0f, sz.y / 2.0f};
    rasterizer.Draw(pipeline, vertex_buffer, uniform_buffer, 3u);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawToDepthBuffer) {
  Application application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();
  pipeline.vertex_descriptor.offset = offsetof(VD, position);
  pipeline.vertex_descriptor.stride = sizeof(VD);
  pipeline.depth_desc.depth_test_enabled = true;
  auto vertex_buffer = Buffer::Create();
  auto uniform_buffer = Buffer::Create();
  vertex_buffer->Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, -1.0}},
      VD{.position = {0.0, 1.0, 1.0}},
      VD{.position = {1.0, -1.0, -1.0}},
  });
  uniform_buffer->Emplace(Uniforms{
      .color = kColorFuchsia,
  });
  static std::shared_ptr<Texture> depth_tex;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    rasterizer.Draw(pipeline, *vertex_buffer, *uniform_buffer, 3u);
    depth_tex = rasterizer.CaptureDebugDepthTexture();
    DisplayTextureInHUD("Depth Buffer", depth_tex.get(), 0.25);
    return true;
  });
  ASSERT_TRUE(Run(application));
  depth_tex = nullptr;
}

TEST_F(RasterizerTest, CanPerformDepthTest) {
  Application application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();
  pipeline.vertex_descriptor.offset = offsetof(VD, position);
  pipeline.vertex_descriptor.stride = sizeof(VD);

  auto buffer = Buffer::Create();
  auto vertex_buffer1 = buffer->Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, -1.0}},
      VD{.position = {0.0, 1.0, 1.0}},
      VD{.position = {1.0, -1.0, -1.0}},
  });
  auto vertex_buffer2 = buffer->Emplace(std::vector<VD>{
      VD{.position = {-1.0, 1.0, -1.0}},  // front
      VD{.position = {1.0, 1.0, -1.0}},   // front
      VD{.position = {0.0, -1.0, 1.0}},   // back
  });
  auto uniform_buffer1 = buffer->Emplace(Uniforms{
      .color = kColorFuchsia,
  });
  auto uniform_buffer2 = buffer->Emplace(Uniforms{
      .color = kColorFirebrick,
  });
  static std::shared_ptr<Texture> depth_tex;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    pipeline.depth_desc.depth_test_enabled = true;
    rasterizer.Draw(pipeline, vertex_buffer1, uniform_buffer1, 3u);
    rasterizer.Draw(pipeline, vertex_buffer2, uniform_buffer2, 3u);
    depth_tex = rasterizer.CaptureDebugDepthTexture();
    DisplayTextureInHUD("Depth Buffer", depth_tex.get(), 0.25);
    return true;
  });
  ASSERT_TRUE(Run(application));
  depth_tex.reset();
}

TEST_F(RasterizerTest, CanShowHUD) {
  Application application;
  application.SetRasterizerCallback([](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    ImGui::ShowDemoWindow();
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanvasCanDrawStuff) {
  Application application;
  auto context = std::make_shared<CanvasContext>();
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorWhite);
    Canvas canvas(context);
    Paint paint;
    paint.color = kColorRed;
    canvas.Translate({10, 10});
    canvas.DrawRect(rasterizer, Rect({100, 100}), paint);
    paint.color = kColorGreen;
    canvas.Translate({100, 100});
    canvas.DrawRect(rasterizer, Rect({100, 100}), paint);
    paint.color = kColorBlue;
    canvas.Translate({100, 100});
    canvas.DrawRect(rasterizer, Rect({100, 100}), paint);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanStencil) {
  Application application;
  auto context = std::make_shared<CanvasContext>();
  static std::shared_ptr<Texture> stencil_tex;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorWhite);
    Canvas canvas(context);
    Paint paint;
    paint.color = kColorRed.WithAlpha(128);
    paint.color_desc = ColorAttachmentDescriptor{.blend.enabled = true};
    paint.stencil_desc = StencilAttachmentDescriptor{
        .stencil_test_enabled = true,
        .stencil_compare = CompareFunction::kAlways,
        .depth_stencil_pass = StencilOperation::kIncrementClamp,
    };
    canvas.Translate({10, 10});
    const auto rect = Rect{{300, 300}};
    const auto offset = glm::vec2{100, 100};
    canvas.DrawRect(rasterizer, rect, paint);
    paint.color = kColorGreen.WithAlpha(128);
    canvas.Translate(offset);
    canvas.DrawRect(rasterizer, rect, paint);
    paint.color = kColorBlue.WithAlpha(128);
    canvas.Translate(offset);
    canvas.DrawRect(rasterizer, rect, paint);
    stencil_tex = rasterizer.CaptureDebugStencilTexture();
    DisplayTextureInHUD("Stencil Buffer", stencil_tex.get(), 0.25);
    return true;
  });
  ASSERT_TRUE(Run(application));
  stencil_tex = nullptr;
}

TEST_F(RasterizerTest, CanClipWithStencils) {
  Application application;
  auto context = std::make_shared<CanvasContext>();
  static std::shared_ptr<Texture> stencil_tex;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorWhite);
    Canvas canvas(context);
    canvas.Translate({10, 10});

    Paint paint;

    // Paint the clip.
    paint.color = kColorRed;
    paint.stencil_desc = StencilAttachmentDescriptor{
        .stencil_test_enabled = true,
        .stencil_compare = CompareFunction::kAlways,
        .depth_stencil_pass = StencilOperation::kIncrementClamp,
    };

    const auto rect = Rect{{300, 300}};
    const auto offset = glm::vec2{200, 100};
    canvas.DrawRect(rasterizer, rect, paint);

    // Draw the box into the clip region.
    paint.color_desc = std::nullopt;
    paint.stencil_desc = StencilAttachmentDescriptor{
        .stencil_test_enabled = true,
        .stencil_compare = CompareFunction::kGreaterEqual,
        .depth_stencil_pass = StencilOperation::kKeep,
    };
    paint.stencil_reference = 1;
    paint.color = kColorGreen;
    canvas.Translate(offset);
    canvas.DrawRect(rasterizer, rect, paint);

    stencil_tex = rasterizer.CaptureDebugStencilTexture();
    DisplayTextureInHUD("Stencil Buffer", stencil_tex.get(), 0.25);
    return true;
  });
  ASSERT_TRUE(Run(application));
  stencil_tex = nullptr;
}

}  // namespace testing
}  // namespace sft
