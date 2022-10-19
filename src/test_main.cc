#include "gtest/gtest.h"
#include "tracing.h"

int main(int argc, char** argv) {
  perfetto::TracingInitArgs trace_args;
  trace_args.backends = perfetto::kInProcessBackend;
  perfetto::Tracing::Initialize(trace_args);
  perfetto::TrackEvent::Register();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
