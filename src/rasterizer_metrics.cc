#include "rasterizer_metrics.h"

#include "imgui.h"

namespace sft {

void RasterizerMetrics::Display() const {
  ImGui::Begin("Rasterizer Metrics");

  ImGui::Text("Size: %d x %d", area.x, area.y);
  ImGui::Text("Draw Count: %zu", draw_count);
  ImGui::Text("Primitives: %zu", primitive_count);
  ImGui::Text("Primitives Processed: %zu (%.0f%%)", primitives_processed,
              primitives_processed * 100.f / primitive_count);
  ImGui::Text("Back Faces Culled: %zu (%.0f%%)", face_culling,
              face_culling * 100.f / primitive_count);
  ImGui::Text("Empty Primitive: %zu (%.0f%%)", empty_primitive,
              empty_primitive * 100.f / primitive_count);
  ImGui::Text("Scissor Culled: %zu (%.0f%%)", scissor_culling,
              scissor_culling * 100.f / primitive_count);
  ImGui::Text("Sample Point Culled: %zu (%.0f%%)", sample_point_culling,
              sample_point_culling * 100.f / primitive_count);
  ImGui::Text("Early Fragment Checks Tripped: %zu", early_fragment_test);
  ImGui::Text("Vertex Invocations: %zu", vertex_invocations);
  ImGui::Text("Fragment Invocations: %zu (%.2fx screen)", fragment_invocations,
              static_cast<ScalarF>(fragment_invocations) / (area.x * area.y));

  ImGui::End();
}

}  // namespace sft
