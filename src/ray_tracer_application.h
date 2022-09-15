#pragma once

#include "application.h"
#include "macros.h"
#include "ray_tracer.h"

namespace sft {

class RayTracerApplication final : public Application {
 public:
  RayTracerApplication()
      : Application(std::make_shared<RayTracer>(glm::ivec2{500, 500})) {
    GetRayTracer()->Render();
  }

  ~RayTracerApplication() {}

  RayTracer* GetRayTracer() const {
    return static_cast<RayTracer*>(renderer_.get());
  }

 private:
  SFT_DISALLOW_COPY_AND_ASSIGN(RayTracerApplication);
};

}  // namespace sft
