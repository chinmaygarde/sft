sft_library(model
  model.cc
  model.h
  model_shader.cc
  model_shader.h
)

find_package(tinyobjloader CONFIG REQUIRED)

target_link_libraries(model
  PUBLIC
    tinyobjloader::tinyobjloader
    rasterizer
)
