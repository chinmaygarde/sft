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

  auto vertex_buffer = std::make_shared<Buffer>();

  vertex_buffer->Emplace(std::vector<VD>{
      {tl, p1},
      {tr, p2},
      {br, p3},
      {br, p3},
      {bl, p4},
      {tl, p1},
  });
  auto texture = std::make_shared<Texture>(SFT_ASSETS_LOCATION "airplane.jpg");
  shader->SetTexture(std::move(texture));
  pipeline->shader = shader;
  pipeline->vertex_descriptor.offset = offsetof(VD, position);
  pipeline->vertex_descriptor.stride = sizeof(VD);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorFirebrick);
    auto uniform_buffer = Buffer{};
    uniform_buffer.Emplace(Uniforms{
        .alpha = 0.75,
    });
    rasterizer.Draw(*pipeline, *vertex_buffer, uniform_buffer, 6);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawModel) {
  RasterizerApplication application;
  Model model(SFT_ASSETS_LOCATION "teapot/teapot.obj");
  ASSERT_TRUE(model.IsValid());
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorGray);
    model.RenderTo(rasterizer);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanCullFaces) {
  RasterizerApplication application;

  Pipeline pipeline;
  pipeline.shader = std::make_shared<ColorShader>();

  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBeige);
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

}  // namespace testing
}  // namespace sft
