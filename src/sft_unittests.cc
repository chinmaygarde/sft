#include <gtest/gtest.h>
#include "Reactor.hpp"
#include "demo_application.h"
#include "runner.h"

namespace sft {
namespace testing {

using RunnerTest = Runner;

TEST(SFTTest, ToyWithReactor) {
  std::cout << "Backend: " << rr::Caps::backendName() << std::endl;
  rr::Function<rr::Float(rr::Float)> myfunc;
  {
    rr::Float val = myfunc.Arg<0>().rvalue() * 2.0f;
    Return(val);
  }
  auto routine = myfunc("myfunc");
  auto entry = (float (*)(float))routine->getEntry();
  ASSERT_FLOAT_EQ(entry(2.0f), 4.0f);
}

TEST_F(RunnerTest, CanRun) {
  DemoApplication application;
  ASSERT_TRUE(Run(application));
}

}  // namespace testing
}  // namespace sft
