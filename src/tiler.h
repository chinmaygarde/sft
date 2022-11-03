#pragma once

#include "geom.h"
#include "macros.h"
#include "pipeline.h"
#include "vertex_data.h"

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

  Tiler();

  ~Tiler();

  void Reset();

  void AddData(Data p_data);

  void Dispatch(Rasterizer& rasterizer);

 private:
  std::vector<Data> data_;
  RTree<size_t, int, 2> tree_;
  glm::ivec2 min_ = {INT_MAX, INT_MAX};
  glm::ivec2 max_ = {INT_MIN, INT_MIN};

  SFT_DISALLOW_COPY_AND_ASSIGN(Tiler);
};

}  // namespace sft
