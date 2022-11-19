#include <thread>
#include "gtest/gtest.h"

namespace sft {

bool RunTests(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS() == EXIT_SUCCESS ? true : false;
}

}  // namespace sft

int main(int argc, char** argv) {
  return sft::RunTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
