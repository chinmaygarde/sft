#include <benchmark/benchmark.h>

namespace sft {

#define DEF_BENCH(name)                           \
  static void BM_##name(benchmark::State& state); \
  BENCHMARK(BM_##name);                           \
  static void BM_##name(benchmark::State& state)

DEF_BENCH(SomeFunction) {
  for (auto _ : state) {
  }
}

}  // namespace sft

BENCHMARK_MAIN();
