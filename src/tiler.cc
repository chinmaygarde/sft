#include "tiler.h"

#include <set>

#include "rasterizer.h"

namespace sft {

void Tiler::Dispatch(Rasterizer& rasterizer) {
  const glm::ivec2 num_slices = {16, 16};
  const glm::ivec2 full_span = {max_.x - min_.x, max_.y - min_.y};
  const glm::ivec2 min_span = {256, 256};
  const glm::ivec2 span = glm::max(
      glm::ivec2{full_span.x / num_slices.x, full_span.y / num_slices.y},
      min_span);

  if (tree_.Count() == 0 || full_span.x <= 0 || full_span.y <= 0) {
    return;
  }

  using IndexSet = std::set<size_t, std::less<size_t>>;
  IndexSet index_set;

  for (auto x = min_.x; x < max_.x; x += span.x) {
    for (auto y = min_.y; y < max_.y; y += span.y) {
      const int a_min[2] = {x, y};
      const int a_max[2] = {x + span.x, y + span.y};
      auto found = tree_.Search(
          a_min, a_max,
          [](size_t idx, void* ctx) -> bool {
            reinterpret_cast<IndexSet*>(ctx)->insert(idx);
            return true;
          },
          &index_set);
      if (found > 0) {
        const auto tile =
            Rect::MakeLTRB(a_min[0], a_min[1], a_max[0], a_max[1]);
        std::cout << "> ";
        for (const auto& index : index_set) {
          std::cout << index << ", ";
          rasterizer.ShadeFragments(data_.at(index), tile);
        }
        std::cout << std::endl;
        index_set.clear();
      }
    }
  }
}

}  // namespace sft
