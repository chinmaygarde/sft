#pragma once

#include "gtest/gtest.h"
#include "macros.h"
#include "timing.h"

namespace sft {

class Application;

class Runner : public ::testing::Test {
 public:
  Runner();

  ~Runner();

  bool Run(Application& application) const;

  SecondsF ElapsedTime() const;

 private:
  TimePoint start_time_;

  SFT_DISALLOW_COPY_AND_ASSIGN(Runner);
};

}  // namespace sft
