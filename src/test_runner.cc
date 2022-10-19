#include "test_runner.h"

#include <SDL.h>

#include <fstream>
#include <sstream>

#include "application.h"
#include "backends/imgui_impl_sdl.h"
#include "fixtures_location.h"
#include "imgui.h"
#include "perfetto.h"

namespace sft {

TestRunner::TestRunner() : start_time_(Clock::now()) {
  SFT_ASSERT(::SDL_Init(SDL_INIT_VIDEO) == 0);
}

SecondsF TestRunner::ElapsedTime() const {
  return Clock::now() - start_time_;
}

static std::string CreateTestName() {
  std::stringstream stream;
  stream << ::testing::UnitTest::GetInstance()
                ->current_test_info()
                ->test_suite_name();
  stream << ".";
  stream << ::testing::UnitTest::GetInstance()->current_test_info()->name();
  return stream.str();
}

static std::string CreateTraceFilePath() {
  std::stringstream stream;
  stream << SFT_ASSETS_LOCATION
         << ::testing::UnitTest::GetInstance()->current_test_info()->name()
         << ".perfetto-trace";
  return stream.str();
}

static bool gSkipRemainingTests = false;

bool TestRunner::Run(Application& application) const {
  TRACE_EVENT(kTraceCategoryTest, "TestRunner::Run");
  bool is_running = true;
  bool success = true;
  application.SetTitle(CreateTestName());
  while (is_running) {
    success = is_running = application.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      auto& io = ImGui::GetIO();

      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDL_KeyCode::SDLK_q:
            case SDL_KeyCode::SDLK_ESCAPE:
              if ((event.key.keysym.mod & KMOD_LSHIFT) ||
                  (event.key.keysym.mod & KMOD_RSHIFT) ||
                  (event.key.keysym.mod & KMOD_LCTRL) ||
                  (event.key.keysym.mod & KMOD_RCTRL)) {
                gSkipRemainingTests = true;
              }
              is_running = false;
              break;
            default:
              break;
          }
          break;
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              if (!application.OnWindowSizeChanged(
                      {event.window.data1, event.window.data2})) {
                std::cout << "Window resizing failed." << std::endl;
                is_running = false;
              }

              break;
          }
          break;
      }
    }
  }
  return success;
}

TestRunner::~TestRunner() {
  ::SDL_Quit();
}

void TestRunner::SetUp() {
  if (gSkipRemainingTests) {
    GTEST_SKIP();
  }
  StartTracing();
}

void TestRunner::TearDown() {
  StopTracing();
}

void TestRunner::StartTracing() {
  if (session_) {
    return;
  }

  perfetto::TraceConfig trace;
  trace.add_buffers()->set_size_kb(5 * 1024);
  auto* data_source_config = trace.add_data_sources()->mutable_config();
  data_source_config->set_name("track_event");

  session_ = perfetto::Tracing::NewTrace();
  session_->Setup(trace);
  session_->StartBlocking();

  perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();
  perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();
  desc.mutable_process()->set_process_name("SFT");
  perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);
}

void TestRunner::StopTracing() {
  if (!session_) {
    return;
  }
  perfetto::TrackEvent::Flush();
  auto session = std::move(session_);
  session->StopBlocking();
  const auto trace_data = session->ReadTraceBlocking();
  std::ofstream trace_stream;
  trace_stream.open(CreateTraceFilePath().c_str(),
                    std::ios::trunc | std::ios::out | std::ios::binary);
  trace_stream.write(&trace_data[0], trace_data.size());
  trace_stream.close();
}

}  // namespace sft
