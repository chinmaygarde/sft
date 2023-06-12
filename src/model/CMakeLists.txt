sft_library(model
  model.cc
  model.h
  model_shader.cc
  model_shader.h
)

target_link_libraries(model
  PUBLIC
    tinyobjloader
    rasterizer
)
