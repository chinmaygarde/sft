#include <gtest/gtest.h>

#include "buffer.h"
#include "fixtures_location.h"
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
  Buffer vertex_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
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
  pipeline->blend_mode = BlendMode::kSourceOver;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    {
      Buffer uniform_buffer;
      uniform_buffer.Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0, 0},
      });
      shader->SetTexture(texture1);
      rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6);
    }
    {
      Buffer uniform_buffer;
      uniform_buffer.Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0.2, 0.2},
      });
      shader->SetTexture(texture2);
      rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6);
    }
    {
      Buffer uniform_buffer;
      uniform_buffer.Emplace(Uniforms{
          .alpha = 0.75,
          .offset = {0.4, 0.4},
      });
      shader->SetTexture(texture3);
      rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6);
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
  Buffer vertex_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });
  Buffer uniform_buffer;
  uniform_buffer.Emplace(Uniforms{
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
  pipeline->blend_mode = BlendMode::kSourceOver;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    { rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6); }
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
  Buffer vertex_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });
  Buffer uniform_buffer;
  uniform_buffer.Emplace(Uniforms{
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
  pipeline->blend_mode = BlendMode::kSourceOver;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    { rasterizer.Draw(*pipeline, vertex_buffer, uniform_buffer, 6); }
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawTeapot) {
  RasterizerApplication application;
  Model model(SFT_ASSETS_LOCATION "teapot/teapot.obj",
              SFT_ASSETS_LOCATION "teapot");
  model.SetScale(4);
  model.SetTexture(std::make_shared<Texture>(SFT_ASSETS_LOCATION "marble.jpg"));
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
  model.SetScale(300);
  model.SetTexture(
      std::make_shared<Texture>(SFT_ASSETS_LOCATION "helmet/Base.png"));
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
  Buffer vertex_buffer, uniform_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, 0.0}},
      VD{.position = {0.0, 1.0, 0.0}},
      VD{.position = {1.0, -1.0, 0.0}},
  });
  uniform_buffer.Emplace(Uniforms{
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
  Buffer vertex_buffer, uniform_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, 0.0}},
      VD{.position = {0.0, 1.0, 0.0}},
      VD{.position = {1.0, -1.0, 0.0}},
  });
  uniform_buffer.Emplace(Uniforms{
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
  Buffer vertex_buffer, uniform_buffer;
  vertex_buffer.Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, -1.0}},
      VD{.position = {0.0, 1.0, 1.0}},
      VD{.position = {1.0, -1.0, -1.0}},
  });
  uniform_buffer.Emplace(Uniforms{
      .color = kColorFuchsia,
  });
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
    rasterizer.Draw(pipeline, vertex_buffer, uniform_buffer, 3u);
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

  Buffer uniform_buffer1, uniform_buffer2;
  Buffer vertex_buffer1, vertex_buffer2;
  vertex_buffer1.Emplace(std::vector<VD>{
      VD{.position = {-1.0, -1.0, -1.0}},
      VD{.position = {0.0, 1.0, 1.0}},
      VD{.position = {1.0, -1.0, -1.0}},
  });
  vertex_buffer2.Emplace(std::vector<VD>{
      VD{.position = {-1.0, 1.0, -1.0}},  // front
      VD{.position = {1.0, 1.0, -1.0}},   // front
      VD{.position = {0.0, -1.0, 1.0}},   // back
  });
  uniform_buffer1.Emplace(Uniforms{
      .color = kColorFuchsia,
  });
  uniform_buffer2.Emplace(Uniforms{
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

}  // namespace testing
}  // namespace sft
