#include "benchmark/benchmark.h"
#include "fixtures_location.h"
#include "model.h"
#include "rasterizer.h"

namespace sft {

static void BM_RasterizerCreation(benchmark::State& state) {
  while (state.KeepRunning()) {
    Rasterizer rasterizer({1024, 768}, SampleCount::kOne);
  }
}
BENCHMARK(BM_RasterizerCreation)->Unit(benchmark::TimeUnit::kMillisecond);

static void BM_LoadModel(benchmark::State& state) {
  while (state.KeepRunning()) {
    Model model(SFT_ASSETS_LOCATION "helmet/Helmet.obj",
                SFT_ASSETS_LOCATION "helmet");
  }
}
BENCHMARK(BM_LoadModel)->Unit(benchmark::TimeUnit::kMillisecond);

static void BM_RenderHelmet(benchmark::State& state) {
  Rasterizer rasterizer({1024, 768}, SampleCount::kOne);
  Model model(SFT_ASSETS_LOCATION "helmet/Helmet.obj",
              SFT_ASSETS_LOCATION "helmet");
  model.SetRotation(45);
  model.SetScale(900);
  auto texture = Image::Create(SFT_ASSETS_LOCATION "helmet/Base.png");
  texture->SetSampler({.min_mag_filter = Filter::kNearest});
  model.SetTexture(texture);
  while (state.KeepRunning()) {
    rasterizer.Clear(kColorGray);
    model.RenderTo(rasterizer);
    auto pixels = rasterizer.GetRenderPass().color.texture->Get({}, 0);
    benchmark::DoNotOptimize(pixels);
  }
}
BENCHMARK(BM_RenderHelmet)->Unit(benchmark::TimeUnit::kMillisecond);

static void BM_SampleLinear(benchmark::State& state) {
  auto texture = Image::Create(SFT_ASSETS_LOCATION "helmet/Base.png");
  texture->SetSampler({.min_mag_filter = Filter::kLinear});
  while (state.KeepRunning()) {
    for (auto i = 0.0f; i <= 1.0f; i += 0.001f) {
      for (auto j = 0.0f; j <= 1.0f; j += 0.001f) {
        auto value = texture->Sample({j, i});
        benchmark::DoNotOptimize(value);
      }
    }
  }
}
BENCHMARK(BM_SampleLinear)->Unit(benchmark::TimeUnit::kMillisecond);

static void BM_SampleNearest(benchmark::State& state) {
  auto texture = Image::Create(SFT_ASSETS_LOCATION "helmet/Base.png");
  texture->SetSampler({.min_mag_filter = Filter::kNearest});
  while (state.KeepRunning()) {
    for (auto i = 0.0f; i <= 1.0f; i += 0.001f) {
      for (auto j = 0.0f; j <= 1.0f; j += 0.001f) {
        auto value = texture->Sample({j, i});
        benchmark::DoNotOptimize(value);
      }
    }
  }
}
BENCHMARK(BM_SampleNearest)->Unit(benchmark::TimeUnit::kMillisecond);

}  // namespace sft

BENCHMARK_MAIN();
