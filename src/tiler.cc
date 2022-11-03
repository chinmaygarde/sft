#include "tiler.h"

#include <set>
#include <vector>

#include "rasterizer.h"

namespace sft {

Tiler::Tiler() = default;

Tiler::~Tiler() = default;

void Tiler::AddData(FragmentResources frag_resources) {
  const auto data = frag_resources_.emplace_back(std::move(frag_resources));
  const auto min = glm::ivec2{glm::floor(data.box.GetLT())};
  const auto max = glm::ivec2{glm::ceil(data.box.GetRB())};
  tree_.Insert((int*)&min, (int*)&max, frag_resources_.size() - 1u);
  min_ = glm::min(min, min_);
  max_ = glm::max(max, max_);
}

void Tiler::Dispatch(Rasterizer& rasterizer) {
  const glm::ivec2 num_slices = {16, 16};
  const glm::ivec2 full_span = max_ - min_;
  const glm::ivec2 min_span = {256, 256};
  const glm::ivec2 span = glm::max(full_span.x / num_slices, min_span);

  if (tree_.Count() == 0 || full_span.x <= 0 || full_span.y <= 0) {
    return;
  }

  using IndexSet = std::vector<size_t>;
  IndexSet index_set;

  for (auto x = min_.x; x < max_.x; x += span.x) {
    for (auto y = min_.y; y < max_.y; y += span.y) {
      const auto min = glm::ivec2{x, y};
      const auto max = min + span;
      auto found = tree_.Search(
          (int*)&min, (int*)&max,
          [](size_t idx, void* ctx) -> bool {
            reinterpret_cast<decltype(index_set)*>(ctx)->push_back(idx);
            return true;
          },
          &index_set);
      if (found == 0) {
        // The bounding boxes of no primitives intersect this tile.
        continue;
      }
      const auto tile = Rect::MakeLTRB(min.x, min.y, max.x, max.y);
      // std::cout << "> ";
      for (const auto& index : index_set) {
        // std::cout << index << ", ";
        rasterizer.ShadeFragments(frag_resources_.at(index), tile);
        // break;
      }
      // std::cout << std::endl;
      index_set.clear();
    }
  }
}

void Tiler::Reset() {
  frag_resources_.clear();
  tree_.RemoveAll();
  min_ = {INT_MAX, INT_MAX};
  max_ = {INT_MIN, INT_MIN};
}

}  // namespace sft
