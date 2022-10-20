#include <thread>
#include "gtest/gtest.h"
#include "tracing.h"

namespace sft {

bool RunTests(int argc, char** argv) {
  TRACE_EVENT(kTraceCategoryTest, __FUNCTION__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS() == EXIT_SUCCESS ? true : false;
}

}  // namespace sft

int main(int argc, char** argv) {
  sft::StartTracing();
  auto result = sft::RunTests(argc, argv);
  sft::StopTracing();
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
