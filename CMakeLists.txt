cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools")

project(sft)

add_subdirectory(third_party/sdl)
add_subdirectory(third_party/tinyobjloader)
add_subdirectory(third_party/googletest)
set(GOOGLETEST_PATH "third_party/googletest")
add_subdirectory(third_party/googlebenchmark)
add_subdirectory(third_party/glm)
set(SWIFTSHADER_ENABLE_ASTC FALSE)
set(SWIFTSHADER_BUILD_TESTS FALSE)
add_subdirectory(third_party/swiftshader)
add_subdirectory(third_party/swiftshader/third_party/glslang)
set(SPIRV_CROSS_ENABLE_TESTS FALSE)
add_subdirectory(third_party/spirv_cross)

include(shaders)

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
  src/shaders/color_shader.cc
  src/shaders/color_shader.h
  src/shaders/impeller_shader.cc
  src/shaders/impeller_shader.h
  src/shaders/simple_shader.cc
  src/shaders/simple_shader.h
  src/shaders/texture_shader.cc
  src/shaders/texture_shader.h
  src/texture.cc
  src/texture.h
  third_party/spirv_cross/include/spirv_cross/internal_interface.cpp
)

target_compile_options(sft_lib PRIVATE -Werror -Wall -Wextra -Wpedantic -Wno-unused-parameter)

target_link_libraries(sft_lib
  PUBLIC
    SDL2
    tinyobjloader
    glm
    ReactorLLVM
    spirv-cross-cpp
)

shader(sft_lib assets/impeller.vert)
shader(sft_lib assets/impeller.frag)

target_include_directories(sft_lib
  PUBLIC
    src
    third_party/stb
    third_party/spirv_cross/include
)

add_executable(sft src/main.cc)

target_link_libraries(sft sft_lib)

add_executable(sft_unittests
  src/sft_unittests.cc
)

target_link_libraries(sft_unittests sft_lib gtest_main)
