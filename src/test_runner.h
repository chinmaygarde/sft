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

  void TearDown() override;

 private:
  TimePoint start_time_;
  std::unique_ptr<perfetto::TracingSession> session_;

  void StartTracing();

  void StopTracing();

  SFT_DISALLOW_COPY_AND_ASSIGN(TestRunner);
};

}  // namespace sft
