sft_library(geometry
  color.cc
  color.h
  geometry.cc
  geometry.h
  rect.cc
  rect.h
  size.cc
  size.h
  types.cc
  types.h
)

target_link_libraries(geometry
  PUBLIC
    glm
)
