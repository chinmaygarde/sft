#include <gtest/gtest.h>

#include "buffer.h"
#include "fixtures_location.h"
#include "imgui.h"
#include "model.h"
#include "pipeline.h"
#include "rasterizer.h"
#include "rasterizer_application.h"
#include "ray_tracer_application.h"
#include "runner.h"
#include "shaders/color_shader.h"
#include "shaders/texture_shader.h"

namespace sft {
namespace testing {

using RayTracerTest = Runner;
using RasterizerTest = Runner;

TEST_F(RasterizerTest, CanClearRasterizer) {
  RasterizerApplication application;
  application.SetRasterizerCallback([](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFuchsia);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawTexturedImage) {
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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

TEST_F(RasterizerTest, CanDrawWithIndexBuffer32Bit) {
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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
  RasterizerApplication application;

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
  pipeline->blend.enabled = true;
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
  RasterizerApplication application;
  Model model(SFT_ASSETS_LOCATION "teapot/teapot.obj",
              SFT_ASSETS_LOCATION "teapot");
  model.SetScale(4);
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

TEST_F(RasterizerTest, CanDrawHelmet) {
  RasterizerApplication application;
  Model model(SFT_ASSETS_LOCATION "helmet/Helmet.obj",
              SFT_ASSETS_LOCATION "helmet");

  auto texture =
      std::make_shared<Texture>(SFT_ASSETS_LOCATION "helmet/Base.png");
  texture->SetSampler({.min_mag_filter = Filter::kNearest});
  model.SetTexture(texture);
  ASSERT_TRUE(model.IsValid());
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    static float rotation = 45;
    static float scale = 300;
    ImGui::SliderFloat("Rotation", &rotation, 0, 360);
    ImGui::SliderFloat("Scale", &scale, 50, 900);
    model.SetRotation(rotation);
    model.SetScale(scale);
    model.RenderTo(rasterizer);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanCullFaces) {
  RasterizerApplication application;

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
  RasterizerApplication application;

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

TEST_F(RayTracerTest, CanRunRaytracer) {
  RayTracerApplication application;
  Sphere sphere;
  sphere.center = {0.0, 0.0, -1.0};
  sphere.radius = 0.5;
  application.GetRayTracer()->AddShape(sphere);
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawToDepthBuffer) {
  RasterizerApplication application;

  using VD = ColorShader::VertexData;
  using Uniforms = ColorShader::Uniforms;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();
  pipeline.vertex_descriptor.offset = offsetof(VD, position);
  pipeline.vertex_descriptor.stride = sizeof(VD);
  pipeline.depth_test_enabled = true;
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
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    rasterizer.Draw(pipeline, *vertex_buffer, *uniform_buffer, 3u);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanPerformDepthTest) {
  RasterizerApplication application;

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
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    pipeline.depth_test_enabled = true;
    rasterizer.Draw(pipeline, vertex_buffer1, uniform_buffer1, 3u);
    rasterizer.Draw(pipeline, vertex_buffer2, uniform_buffer2, 3u);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanShowHUD) {
  RasterizerApplication application;
  application.SetRasterizerCallback([](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    ImGui::ShowDemoWindow();
    return true;
  });
  ASSERT_TRUE(Run(application));
}

}  // namespace testing
}  // namespace sft
