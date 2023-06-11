/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include "gtest/gtest.h"
#include "macros.h"
#include "timing.h"

namespace sft {

class Playground;

class PlaygroundTest : public ::testing::Test {
 public:
  PlaygroundTest();

  ~PlaygroundTest();

  bool Run(Playground& playground) const;

  SecondsF ElapsedTime() const;

  void SetUp() override;

  void TearDown() override;

 private:
  TimePoint start_time_;

  SFT_DISALLOW_COPY_AND_ASSIGN(PlaygroundTest);
};

}  // namespace sft
