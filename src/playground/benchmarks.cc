#include <benchmark/benchmark.h>
#include "rasterizer.h"

namespace sft {

#define ADD_BENCHMARK(name) \
  BENCHMARK_F(Benchmark, name)(benchmark::State & state)

class BenchmarkFixture : public ::benchmark::Fixture {
 public:
  BenchmarkFixture() : scheduler_(marl::Scheduler::Config::allCores()) {}

  void SetUp(const ::benchmark::State& state) override { scheduler_.bind(); }

  void TearDown(const ::benchmark::State& state) override {
    scheduler_.unbind();
  }

 private:
  marl::Scheduler scheduler_;

  SFT_DISALLOW_COPY_AND_ASSIGN(BenchmarkFixture);
};

using Benchmark = BenchmarkFixture;

constexpr glm::ivec2 kDefaultRasterizerSize = {1 << 14, 1 << 14};

ADD_BENCHMARK(DoNothing) {
  for (auto _ : state) {
  }
}

ADD_BENCHMARK(Clear) {
  Rasterizer rasterizer(kDefaultRasterizerSize, SampleCount::kFour);
  for (auto _ : state) {
    rasterizer.ResetMetrics();
    rasterizer.Clear(kColorFirebrick);
    rasterizer.Finish();
  }
}

}  // namespace sft

BENCHMARK_MAIN();
