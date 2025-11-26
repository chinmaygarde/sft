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

find_package(glm CONFIG REQUIRED)

target_link_libraries(geometry
  PUBLIC
    glm::glm
)
