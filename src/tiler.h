#pragma once

#include "geom.h"
#include "macros.h"
#include "pipeline.h"

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
  }

 private:
  std::vector<Data> data_;
  RTree<size_t, int, 2> tree_;

  SFT_DISALLOW_COPY_AND_ASSIGN(Tiler);
};

}  // namespace sft
