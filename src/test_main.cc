#include "gtest/gtest.h"
#include "tracing.h"

int main(int argc, char** argv) {
  sft::StartTracing();
  testing::InitGoogleTest(&argc, argv);
  auto result = RUN_ALL_TESTS();
  sft::StopTracing();
  return result;
}
