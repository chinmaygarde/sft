#include <gtest/gtest.h>

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

TEST_F(RasterizerTest, CanDrawTriangleRasterizer) {
  RasterizerApplication application;
  auto pipeline = std::make_shared<Pipeline>();
  pipeline->shader = std::make_shared<ColorShader>(kColorAzure);
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBlue);
    pipeline->viewport = rasterizer.GetSize();
    rasterizer.SetPipeline(pipeline);
    glm::vec3 p1 = {0.0, 0.5, 0.0};
    glm::vec3 p2 = {-0.5, -0.5, 0.0};
    glm::vec3 p3 = {0.5, -0.5, 0.0};
    rasterizer.DrawTriangle(p1, p2, p3);
    return true;
  });
  ASSERT_TRUE(Run(application));
}

TEST_F(RasterizerTest, CanDrawTexturedImage) {
  RasterizerApplication application;
  auto pipeline = std::make_shared<Pipeline>();
  auto shader = std::make_shared<TextureShader>();
  auto texture = std::make_shared<Texture>("assets/airplane.jpg");
  shader->SetTexture(std::move(texture));
  pipeline->shader = shader;
  application.SetRasterizerCallback([&](Rasterizer& rasterizer) -> bool {
    rasterizer.Clear(kColorBlue);
    pipeline->viewport = rasterizer.GetSize();
    rasterizer.SetPipeline(pipeline);
    glm::vec3 p1 = {-0.5, 0.5, 0.0};
    glm::vec3 p2 = {0.5, 0.5, 0.0};
    glm::vec3 p3 = {0.5, -0.5, 0.0};
    glm::vec3 p4 = {-0.5, -0.5, 0.0};
    rasterizer.DrawTriangle(p1, p2, p3);
    rasterizer.DrawTriangle(p3, p4, p1);
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
