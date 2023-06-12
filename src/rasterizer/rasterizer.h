/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "buffer.h"
#include "buffer_view.h"
#include "geometry.h"
#include "pipeline.h"
#include "rasterizer_metrics.h"
#include "render_pass.h"
#include "stage_resources.h"
#include "texture.h"
#include "tiler.h"

namespace sft {

class Image;

class Rasterizer {
 public:
  Rasterizer(glm::ivec2 size, SampleCount sample_count);

  ~Rasterizer();

  RenderPassAttachments& GetRenderPass();

  glm::ivec2 GetSize() const;

  void Clear(Color color);

  void Finish() { tiler_.Dispatch(*this); }

  void Draw(std::shared_ptr<Pipeline> pipeline,
            const BufferView& vertex_buffer,
            Uniforms uniforms,
            size_t count,
            uint32_t stencil_refernece = 0) {
    return Draw(std::move(pipeline), vertex_buffer, {}, std::move(uniforms),
                count, stencil_refernece);
  }

  void Draw(std::shared_ptr<Pipeline> pipeline,
            const BufferView& vertex_buffer,
            const BufferView& index_buffer,
            Uniforms uniforms,
            size_t count,
            uint32_t stencil_reference = 0) {
    metrics_.draw_count++;
    auto resources = std::make_shared<DispatchResources>();
    resources->vertex = std::move(vertex_buffer);
    resources->index = std::move(index_buffer);
    resources->uniform = std::move(uniforms);
    VertexResources data(pipeline,              //
                         std::move(resources),  //
                         stencil_reference      //
    );
    const auto vtx_offset = pipeline->vertex_descriptor.offset;
    for (size_t i = 0; i < count; i += 3) {
      data.base_vertex_id = i;
      data.vtx[0] = data.LoadVertexData<glm::vec3>(i + 0, vtx_offset);
      data.vtx[1] = data.LoadVertexData<glm::vec3>(i + 1, vtx_offset);
      data.vtx[2] = data.LoadVertexData<glm::vec3>(i + 2, vtx_offset);
      DrawTriangle(data);
    }
  }

  void ResetMetrics();

  const RasterizerMetrics& GetMetrics() const;

  [[nodiscard]] bool Resize(glm::ivec2 size);

  [[nodiscard]] bool ResizeSamples(SampleCount count);

  void ShadeFragments(const FragmentResources& tiler_data, const Rect& tile);

 private:
  RenderPassAttachments pass_;
  glm::ivec2 size_;
  RasterizerMetrics metrics_;
  Tiler tiler_;

  bool FragmentPassesDepthTest(const Pipeline& pipeline,
                               glm::ivec2 pos,
                               ScalarF depth,
                               size_t sample) const;

  bool UpdateAndCheckFragmentPassesStencilTest(const Pipeline& pipeline,
                                               glm::ivec2 pos,
                                               bool depth_test_passes,
                                               uint32_t reference_value,
                                               size_t sample);

  void UpdateColor(const ColorAttachmentDescriptor& color_desc,
                   const glm::ivec2& pos,
                   const Color& color,
                   size_t sample);

  void UpdateDepth(const DepthAttachmentDescriptor& depth_desc,
                   const glm::ivec2& pos,
                   ScalarF depth,
                   size_t sample);

  void DrawTriangle(const VertexResources& data);

  SFT_DISALLOW_COPY_AND_ASSIGN(Rasterizer);
};

}  // namespace sft
