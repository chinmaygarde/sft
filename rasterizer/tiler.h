/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <vector>

#include "geom.h"
#include "macros.h"
#include "pipeline.h"
#include "stage_resources.h"

#define Min std::min
#define Max std::max

#ifndef __cdecl
#define __cdecl
#endif  // __cdecl

#include <RTree.h>

namespace sft {

class Rasterizer;

class Tiler {
 public:
  Tiler();

  ~Tiler();

  void Reset();

  void AddData(FragmentResources frag_resources);

  void Dispatch(Rasterizer& rasterizer);

 private:
  std::vector<FragmentResources> frag_resources_;
  RTree<size_t, int, 2> tree_;
  glm::ivec2 min_ = {INT_MAX, INT_MAX};
  glm::ivec2 max_ = {INT_MIN, INT_MIN};

  SFT_DISALLOW_COPY_AND_ASSIGN(Tiler);
};

}  // namespace sft
