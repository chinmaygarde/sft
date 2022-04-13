#pragma once

#include <tiny_obj_loader.h>
#include <iostream>
#include <string>
#include "image.h"

namespace sft {

class Model {
 public:
  Model(std::string path) {
    std::string warnings;
    std::string errors;
    auto result = tinyobj::LoadObj(&attrib_, &shapes_, &materials_, &warnings,
                                   &errors, path.c_str());

    if (!warnings.empty()) {
      std::cout << warnings << std::endl;
    }

    if (!errors.empty()) {
      std::cout << errors << std::endl;
    }

    if (!result) {
      return;
    }
    is_valid_ = true;
  }

  bool IsValid() const { return is_valid_; }

  void RenderTo(Image& image) {
    if (!IsValid()) {
      return;
    }
    for (const auto& shape : shapes_) {
    }
  }

 private:
  tinyobj::attrib_t attrib_;
  std::vector<tinyobj::shape_t> shapes_;
  std::vector<tinyobj::material_t> materials_;
  bool is_valid_ = false;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
};

}  // namespace sft
