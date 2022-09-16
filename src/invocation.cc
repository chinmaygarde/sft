#include "invocation.h"

#include "rasterizer.h"

namespace sft {

glm::vec2 FragmentInvocation::InterpolateVec2(size_t offset) const {
  return rasterizer.InterpolateVec2(data,                     //
                                    barycentric_coordinates,  //
                                    offset                    //
  );
}

}  // namespace sft
