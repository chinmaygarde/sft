#pragma once

#include "geom.h"
#include "macros.h"
#include "pipeline.h"
#include "triangle_data.h"

#define Min std::min
#define Max std::max

#include "superliminal/RTree.h"

namespace sft {

class Rasterizer;

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
    min_ = {INT_MAX, INT_MAX};
    max_ = {INT_MIN, INT_MIN};
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
    tree_.Insert(a_min, a_max, data_.size() - 1u);
    min_ = glm::min(glm::ivec2{a_min[0], a_min[1]}, min_);
    max_ = glm::max(glm::ivec2{a_max[0], a_max[1]}, max_);
  }

  void Dispatch(Rasterizer& rasterizer);

 private:
  std::vector<Data> data_;
  RTree<size_t, int, 2> tree_;
  glm::ivec2 min_ = {INT_MAX, INT_MAX};
  glm::ivec2 max_ = {INT_MIN, INT_MIN};

  SFT_DISALLOW_COPY_AND_ASSIGN(Tiler);
};

}  // namespace sft
