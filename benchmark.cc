#include <benchmark/benchmark.h>
#include "image.h"

namespace sft {

static void DrawTriangle(benchmark::State& state) {
  // Perform setup here
  auto image = Image{1280, 1280};
  for (auto _ : state) {
    image.DrawTriangle({10, 10}, {10, 800}, {800, 800}, kColorBlack);
    image.DrawLine({10, 10}, {800, 800}, kColorGreen);
  }
}

BENCHMARK(DrawTriangle);

}  // namespace sft

BENCHMARK_MAIN();
