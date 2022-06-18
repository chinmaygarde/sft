#pragma once

#include "shaders/spirv_cross/external_interface.h"

namespace sft {

class SPIRVCrossShader {
 public:
  SPIRVCrossShader(const struct spirv_cross_interface* iface) : iface_(iface) {
    shader_ = iface_->construct();
  }

  ~SPIRVCrossShader() { iface_->destruct(shader_); }

  void Invoke() { iface_->invoke(shader_); }

 private:
  const struct spirv_cross_interface* iface_;
  spirv_cross_shader_t* shader_;
};

};  // namespace sft
