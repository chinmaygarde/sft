#include <gtest/gtest.h>
#include "rasterizer_application.h"
#include "runner.h"

namespace sft {
namespace testing {

using RunnerTest = Runner;

TEST_F(RunnerTest, CanRunRasterizer) {
  RasterizerApplication application;
  ASSERT_TRUE(Run(application));
}

}  // namespace testing
}  // namespace sft
