#include <gtest/gtest.h>
#include "rasterizer_application.h"
#include "ray_tracer_application.h"
#include "runner.h"

namespace sft {
namespace testing {

using RunnerTest = Runner;

TEST_F(RunnerTest, CanRunRasterizer) {
  RasterizerApplication application;
  ASSERT_TRUE(Run(application));
}

TEST_F(RunnerTest, CanRunRaytracer) {
  RayTracerApplication application;
  Sphere sphere;
  sphere.center = {0.0, 0.0, -1.0};
  sphere.radius = 0.5;
  application.GetRayTracer()->AddShape(sphere);
  ASSERT_TRUE(Run(application));
}

}  // namespace testing
}  // namespace sft
