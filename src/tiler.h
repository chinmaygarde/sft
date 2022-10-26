#pragma once

#include "geom.h"
#include "macros.h"
#include "pipeline.h"
#include "triangle_data.h"

#define Min std::min
#define Max std::max

#include "superliminal/RTree.h"

namespace sft {

class Tiler {
 public:
  struct Data {
    Rect box;
    glm::vec3 ndc[3];
    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<Resources> resources;
    uint32_t stencil_reference = 0;
  };

  Tiler() {}

  ~Tiler() {}

  void Reset() {
    data_.clear();
    tree_.RemoveAll();
  }

  void AddData(Data p_data) {
    const auto data = data_.emplace_back(std::move(p_data));
    const auto ltrb = data.box.GetLTRB();
    const int a_min[2] = {
        static_cast<int>(glm::floor(ltrb[0])),
        static_cast<int>(glm::floor(ltrb[1])),
    };
    const int a_max[2] = {
        static_cast<int>(glm::ceil(ltrb[2])),
        static_cast<int>(glm::ceil(ltrb[3])),
    };
    tree_.Insert(a_min, a_max, data_.size());
    min_ = glm::min(glm::ivec2{a_min[0], a_min[1]}, min_);
    max_ = glm::max(glm::ivec2{a_max[0], a_max[1]}, max_);
  }

  void Dispatch() {
    const glm::ivec2 num_slices = {16, 16};
    const glm::ivec2 full_span = {max_.x - min_.x, max_.y - min_.y};
    const glm::ivec2 min_span = {256, 256};
    const glm::ivec2 span = glm::max(
        glm::ivec2{full_span.x / num_slices.x, full_span.y / num_slices.y},
        min_span);

    if (tree_.Count() == 0 || full_span.x <= 0 || full_span.y <= 0) {
      return;
    }

    for (auto x = min_.x; x < max_.x; x += span.x) {
      for (auto y = min_.y; y < max_.y; y += span.y) {
        const int a_min[2] = {x, y};
        const int a_max[2] = {x + span.x, y + span.y};
        auto count = tree_.Search(
            a_min, a_max,
            [](size_t idx, void* context) -> bool { return true; }, nullptr);
        if (count == 0) {
          continue;
        }
      }
    }
  }

 private:
  std::vector<Data> data_;
  RTree<size_t, int, 2> tree_;
  glm::ivec2 min_ = {INT_MAX, INT_MAX};
  glm::ivec2 max_ = {INT_MIN, INT_MIN};

  SFT_DISALLOW_COPY_AND_ASSIGN(Tiler);
};

}  // namespace sft
