#pragma once

#include "gtest/gtest.h"
#include "macros.h"

namespace sft {

class Application;

class Runner : public ::testing::Test {
 public:
  Runner();

  ~Runner();

  bool Run(Application& application) const;

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;

  SFT_DISALLOW_COPY_AND_ASSIGN(Runner);
};

}  // namespace sft
