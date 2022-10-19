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
  cfg.add_buffers()->set_size_kb(1024 * 4);
  auto* ds_cfg = cfg.add_data_sources()->mutable_config();
  ds_cfg->set_name("track_event");

  auto tracing_session = perfetto::Tracing::NewTrace();
  tracing_session->Setup(cfg);
  tracing_session->StartBlocking();

  // Give a custom name for the traced process.
  perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();
  perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();
  desc.mutable_process()->set_process_name("SFT");
  perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);

  gGlobalSession = std::move(tracing_session);
}

void StopTracing() {
  SFT_ASSERT(gGlobalSession);
  // Make sure the last event is closed for this example.
  perfetto::TrackEvent::Flush();

  // Stop tracing and read the trace data.
  gGlobalSession->StopBlocking();
  std::vector<char> trace_data(gGlobalSession->ReadTraceBlocking());

  // Write the result into a file.
  // Note: To save memory with longer traces, you can tell Perfetto to write
  // directly into a file by passing a file descriptor into Setup() above.
  std::ofstream output;
  output.open("last_run.perfetto-trace",
              std::ios::trunc | std::ios::out | std::ios::binary);
  output.write(&trace_data[0], std::streamsize(trace_data.size()));
  output.close();
  gGlobalSession.reset();
}

}  // namespace sft
