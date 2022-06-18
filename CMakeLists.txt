cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sft)

add_subdirectory(third_party/sdl)
add_subdirectory(third_party/tinyobjloader)
add_subdirectory(third_party/googletest)
set(GOOGLETEST_PATH "third_party/googletest")
add_subdirectory(third_party/googlebenchmark)
add_subdirectory(third_party/glm)

add_library(sft_lib
  src/application.cc
  src/application.h
  src/geom.cc
  src/geom.h
  src/model.cc
  src/model.h
  src/pipeline.cc
  src/pipeline.h
  src/rasterizer.cc
  src/rasterizer.h
  src/sdl_utils.cc
  src/sdl_utils.h
  # src/shaders/impeller.frag.gen.h
  # src/shaders/impeller.vert.gen.h
  # src/shaders/impeller.frag.h
  # src/shaders/impeller.vert.h
  # src/shaders/impeller.frag.cc
  # src/shaders/impeller.vert.cc
  src/shaders/simple_shader.cc
  src/shaders/simple_shader.h
  src/shaders/color_shader.cc
  src/shaders/color_shader.h
  src/shaders/spirv_cross
  src/shaders/spirv_cross/barrier.hpp
  src/shaders/spirv_cross/external_interface.h
  src/shaders/spirv_cross/image.hpp
  src/shaders/spirv_cross/internal_interface.hpp
  src/shaders/spirv_cross/sampler.hpp
  src/shaders/spirv_cross/thread_group.hpp
  src/shaders/texture_shader.cc
  src/shaders/texture_shader.h
  src/texture.cc
  src/texture.h
)

target_link_libraries(sft_lib SDL2 tinyobjloader glm)

target_include_directories(sft_lib PUBLIC src third_party/stb)

add_executable(sft src/main.cc)

target_link_libraries(sft sft_lib)
