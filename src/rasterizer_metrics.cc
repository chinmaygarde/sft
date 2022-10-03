#include "rasterizer_metrics.h"

#include "imgui.h"

namespace sft {

void RasterizerMetrics::Display() const {
  ImGui::Begin("Rasterizer Metrics");

  ImGui::Text("Draw Count: %zu", draw_count);
  ImGui::Text("Primitives: %zu", primitive_count);
  ImGui::Text("Back Faces Culled: %zu", face_culling);
  ImGui::Text("Empty Primitive: %zu", empty_primitive);
  ImGui::Text("Scissor Culled: %zu", scissor_culling);
  ImGui::Text("Sample Point Culled: %zu", sample_point_culling);
  ImGui::Text("Early Fragment Checks Tripped: %zu", early_fragment_test);
  ImGui::Text("Vertex Invocations: %zu", vertex_invocations);
  ImGui::Text("Fragment Invocations: %zu", fragment_invocations);

  ImGui::End();
}

}  // namespace sft
