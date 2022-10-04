#pragma once

#include <memory>

#include "attachment.h"
#include "geom.h"
#include "invocation.h"
#include "macros.h"
#include "rasterizer.h"
#include "shader.h"
#include "texture.h"

namespace sft {

class CanvasShader final : public Shader {
 public:
  struct VertexData {
    glm::vec2 position;
    glm::vec2 uv;
  };

  struct Uniforms {
    glm::vec2 size;
    glm::mat4 ctm;
    glm::vec4 color;
  };

  struct Varyings {};

  CanvasShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec3 ProcessVertex(const VertexInvocation& inv) const override {
    auto size = UNIFORM(size);
    auto projection = glm::ortho(0.f, size.x, size.y, 0.f);
    auto ctm = UNIFORM(ctm);
    return projection * ctm * glm::vec4(VTX(position), 0.0, 1.0);
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    return UNIFORM(color);
  }

 private:
  // Texture* texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(CanvasShader);
};

class CanvasContext {
 public:
  CanvasContext() {
    pipeline_.shader = std::make_shared<CanvasShader>();
    pipeline_.vertex_descriptor.stride = sizeof(CanvasShader::VertexData);
    pipeline_.vertex_descriptor.offset =
        offsetof(CanvasShader::VertexData, position);
    pipeline_.vertex_descriptor.index_type = IndexType::kUInt16;
    pipeline_.vertex_descriptor.vertex_format = VertexFormat::kFloat2;
    pipeline_.cull_face = std::nullopt;
  }

  ~CanvasContext() {}

  Pipeline& GetPipeline() { return pipeline_; }

 private:
  Pipeline pipeline_;
  SFT_DISALLOW_COPY_AND_ASSIGN(CanvasContext);
};

struct Paint {
  glm::vec4 color = kColorSkyBlue;
  std::optional<ColorAttachmentDescriptor> color_desc;
  std::optional<DepthAttachmentDescriptor> depth_desc;
  std::optional<StencilAttachmentDescriptor> stencil_desc;
  uint32_t stencil_reference = 0;
};

class Canvas {
 public:
  Canvas(std::shared_ptr<CanvasContext> context)
      : context_(std::move(context)) {}

  void DrawRect(Rasterizer& rasterizer, Rect rect, const Paint& paint) {
    using VD = CanvasShader::VertexData;
    using Uniforms = CanvasShader::Uniforms;
    auto buffer = Buffer::Create();
    auto vertex_buffer = buffer->Emplace(std::vector<VD>{
        VD{.position = rect.origin, .uv = {}},
        VD{.position = rect.origin + glm::vec2{rect.size.width, 0.0}, .uv = {}},
        VD{.position =
               rect.origin + glm::vec2{rect.size.width, rect.size.height},
           .uv = {}},
        VD{.position = rect.origin + glm::vec2{0, rect.size.height}, .uv = {}},
    });
    auto uniform_buffer = buffer->Emplace<Uniforms>(Uniforms{
        .size = rasterizer.GetSize(), .color = paint.color, .ctm = ctm_});
    auto index_buffer =
        buffer->Emplace(std::vector<uint16_t>{0, 1, 2, 2, 3, 0});

    auto& pipeline = context_->GetPipeline();

    pipeline.color_desc =
        paint.color_desc.value_or(ColorAttachmentDescriptor{});

    pipeline.depth_desc =
        paint.depth_desc.value_or(DepthAttachmentDescriptor{});

    pipeline.stencil_desc =
        paint.stencil_desc.value_or(StencilAttachmentDescriptor{});

    rasterizer.Draw(pipeline,                //
                    vertex_buffer,           //
                    index_buffer,            //
                    uniform_buffer,          //
                    6,                       //
                    paint.stencil_reference  //
    );
  }

  void Translate(glm::vec2 tx) {
    ctm_ = glm::translate(ctm_, glm::vec3{tx, 0.0});
  }

 private:
  std::shared_ptr<CanvasContext> context_;
  glm::mat4 ctm_ = glm::identity<glm::mat4>();

  SFT_DISALLOW_COPY_AND_ASSIGN(Canvas);
};

}  // namespace sft
