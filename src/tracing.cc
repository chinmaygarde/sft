#include "tracing.h"

#include <fstream>

#include "macros.h"

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace sft {

static std::unique_ptr<perfetto::TracingSession> gGlobalSession;

static void InitializePerfetto() {
  perfetto::TracingInitArgs args;
  // The backends determine where trace events are recorded. For this example we
  // are going to use the in-process tracing service, which only includes in-app
  // events.
  args.backends = perfetto::kInProcessBackend;

  perfetto::Tracing::Initialize(args);
  perfetto::TrackEvent::Register();
}

void StartTracing() {
  SFT_ASSERT(!gGlobalSession);
  InitializePerfetto();

  // The trace config defines which types of data sources are enabled for
  // recording. In this example we just need the "track_event" data source,
  // which corresponds to the TRACE_EVENT trace points.
  perfetto::TraceConfig cfg;
  cfg.add_buffers()->set_size_kb(1024);
  auto* ds_cfg = cfg.add_data_sources()->mutable_config();
  ds_cfg->set_name("track_event");

  gGlobalSession = perfetto::Tracing::NewTrace(perfetto::kInProcessBackend);
  gGlobalSession->Setup(cfg);
  gGlobalSession->StartBlocking();
  // Give a custom name for the traced process.
  perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();
  perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();
  desc.mutable_process()->set_process_name("SFT Harness");
  perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);
}

void StopTracing() {
  SFT_ASSERT(gGlobalSession.get());
  // Make sure the last event is closed for this example.
  perfetto::TrackEvent::Flush();

  // Stop tracing and read the trace data.
  gGlobalSession->StopBlocking();
  std::vector<char> trace_data(gGlobalSession->ReadTraceBlocking());

  // Write the result into a file.
  static constexpr const char* kTraceFileName = "last_run.perfetto-trace";
  std::ofstream output;
  output.open(kTraceFileName, std::ios::out | std::ios::binary);
  output.write(&trace_data[0], std::streamsize(trace_data.size()));
  output.close();
  gGlobalSession.reset();
  std::cout << "Trace session written to \"" << kTraceFileName << "\" ("
            << trace_data.size() << " bytes)." << std::endl;
}

}  // namespace sft
