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
    glm::mat4 xformation;
    glm::vec4 color;
  };

  struct Varyings {
    glm::vec2 uv;
  };

  CanvasShader() = default;

  size_t GetVaryingsSize() const override { return sizeof(Varyings); }

  glm::vec4 ProcessVertex(const VertexInvocation& inv) const override {
    FORWARD(uv, uv);
    return UNIFORM(xformation) * glm::vec4(VTX(position), 0.0, 1.0);
  }

  glm::vec4 ProcessFragment(const FragmentInvocation& inv) const override {
    auto color = UNIFORM(color);
    if (texture_) {
      color *= texture_->Sample(VARYING_LOAD(uv));
    }
    return color;
  }

  void SetTexture(const Texture* texture) { texture_ = texture; }

 private:
  const Texture* texture_;

  SFT_DISALLOW_COPY_AND_ASSIGN(CanvasShader);
};

class CanvasContext {
 public:
  CanvasContext() {
    shader_ = std::make_shared<CanvasShader>();
    pipeline_.shader = shader_;
    pipeline_.vertex_descriptor.stride = sizeof(CanvasShader::VertexData);
    pipeline_.vertex_descriptor.offset =
        offsetof(CanvasShader::VertexData, position);
    pipeline_.vertex_descriptor.index_type = IndexType::kUInt16;
    pipeline_.vertex_descriptor.vertex_format = VertexFormat::kFloat2;
    pipeline_.cull_face = std::nullopt;
  }

  ~CanvasContext() {}

  Pipeline& GetPipeline() { return pipeline_; }

  void SetTexture(const Texture* texture) { shader_->SetTexture(texture); }

 private:
  std::shared_ptr<CanvasShader> shader_;
  Pipeline pipeline_;
  SFT_DISALLOW_COPY_AND_ASSIGN(CanvasContext);
};

struct Paint {
  glm::vec4 color = kColorWhite;
  std::optional<ColorAttachmentDescriptor> color_desc;
  std::optional<DepthAttachmentDescriptor> depth_desc;
  std::optional<StencilAttachmentDescriptor> stencil_desc;
  std::shared_ptr<Texture> texture;
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
        VD{.position = rect.origin, .uv = {0, 0}},
        VD{.position = rect.origin + glm::vec2{rect.size.width, 0.0},
           .uv = {1, 0}},
        VD{.position =
               rect.origin + glm::vec2{rect.size.width, rect.size.height},
           .uv = {1, 1}},
        VD{.position = rect.origin + glm::vec2{0, rect.size.height},
           .uv = {0, 1}},
    });

    auto size = glm::vec2{rasterizer.GetSize()};
    auto projection = glm::ortho(0.f, size.x, size.y, 0.f);
    auto uniform_buffer = buffer->Emplace<Uniforms>(
        Uniforms{.color = paint.color, .xformation = projection * ctm_});
    auto index_buffer =
        buffer->Emplace(std::vector<uint16_t>{0, 1, 2, 2, 3, 0});

    auto& pipeline = context_->GetPipeline();

    pipeline.color_desc =
        paint.color_desc.value_or(ColorAttachmentDescriptor{});

    pipeline.depth_desc =
        paint.depth_desc.value_or(DepthAttachmentDescriptor{});

    pipeline.stencil_desc =
        paint.stencil_desc.value_or(StencilAttachmentDescriptor{});

    context_->SetTexture(paint.texture.get());

    rasterizer.Draw(pipeline,                //
                    vertex_buffer,           //
                    index_buffer,            //
                    uniform_buffer,          //
                    6,                       //
                    paint.stencil_reference  //
    );

    context_->SetTexture(nullptr);
  }

  void Translate(glm::vec2 tx) {
    ctm_ = glm::translate(ctm_, glm::vec3{tx, 0.0});
  }

  void Concat(const glm::mat4& xform) { ctm_ *= xform; }

 private:
  std::shared_ptr<CanvasContext> context_;
  glm::mat4 ctm_ = glm::identity<glm::mat4>();

  SFT_DISALLOW_COPY_AND_ASSIGN(Canvas);
};

}  // namespace sft
