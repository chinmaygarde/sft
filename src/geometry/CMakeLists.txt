sft_library(geometry
  color.cc
  color.h
  geometry.cc
  geometry.h
  types.cc
  types.h
)

target_link_libraries(geometry
  PUBLIC
    glm
)
