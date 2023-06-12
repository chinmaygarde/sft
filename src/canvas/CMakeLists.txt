sft_library(canvas
  canvas.cc
  canvas.h
  color_shader.cc
  color_shader.h
  paint.cc
  paint.h
  texture_shader.cc
  texture_shader.h
)

target_link_libraries(canvas
  PUBLIC
    geometry
    rasterizer
)
