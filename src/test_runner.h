#pragma once

#include "gtest/gtest.h"
#include "macros.h"
#include "timing.h"

namespace sft {

class Application;

class TestRunner : public ::testing::Test {
 public:
  TestRunner();

  ~TestRunner();

  bool Run(Application& application) const;

  SecondsF ElapsedTime() const;

  void SetUp() override;

 private:
  TimePoint start_time_;

  SFT_DISALLOW_COPY_AND_ASSIGN(TestRunner);
};

}  // namespace sft
