cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM NAMES ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools")

project(sft)

enable_testing()

set(SPIRV_SKIP_TESTS FALSE)
add_subdirectory(third_party/sdl)
add_subdirectory(third_party/tinyobjloader)
add_subdirectory(third_party/googletest)
set(GOOGLETEST_PATH "third_party/googletest")
set(BENCHMARK_ENABLE_TESTING FALSE)
add_subdirectory(third_party/googlebenchmark)
add_subdirectory(third_party/glm)

include(GoogleTest)

add_library(sft_lib
  src/application.cc
  src/application.h
  src/attachment.cc
  src/attachment.h
  src/blend.cc
  src/blend.h
  src/buffer.cc
  src/buffer.h
  src/buffer_view.cc
  src/buffer_view.h
  src/geom.cc
  src/geom.h
  src/imgui_impl_sft.cc
  src/imgui_impl_sft.h
  src/invocation.cc
  src/invocation.h
  src/mapping.cc
  src/mapping.h
  src/model.cc
  src/model.h
  src/pipeline.cc
  src/pipeline.h
  src/rasterizer.cc
  src/rasterizer.h
  src/rasterizer_application.cc
  src/rasterizer_application.h
  src/rasterizer_metrics.cc
  src/rasterizer_metrics.h
  src/ray_tracer.cc
  src/ray_tracer.h
  src/ray_tracer_application.cc
  src/ray_tracer_application.h
  src/renderer.cc
  src/renderer.h
  src/sampler.cc
  src/sampler.h
  src/sdl_utils.cc
  src/sdl_utils.h
  src/shaders/color_shader.cc
  src/shaders/color_shader.h
  src/shaders/imgui_shader.cc
  src/shaders/imgui_shader.h
  src/shaders/model_shader.cc
  src/shaders/model_shader.h
  src/shaders/texture_shader.cc
  src/shaders/texture_shader.h
  src/sphere.cc
  src/sphere.h
  src/texture.cc
  src/texture.h
  src/timing.cc
  src/timing.h
  src/triangle_data.cc
  src/triangle_data.h
  src/vertex_descriptor.cc
  src/vertex_descriptor.h
  third_party/imgui/backends/imgui_impl_sdl.cpp
  third_party/imgui/backends/imgui_impl_sdl.h
  third_party/imgui/imgui.cpp
  third_party/imgui/imgui.h
  third_party/imgui/imgui_demo.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_tables.cpp
  third_party/imgui/imgui_widgets.cpp
)

target_compile_options(sft_lib
  PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

target_link_libraries(sft_lib
  PUBLIC
    SDL2
    tinyobjloader
    glm
)

target_include_directories(sft_lib
  PUBLIC
    src
    third_party/stb
    third_party/imgui
    third_party/spirv_cross/include
)

add_executable(sft_unittests
  src/sft_unittests.cc
  src/runner.h
  src/runner.cc
)

target_link_libraries(sft_unittests sft_lib gtest_main)

get_filename_component(SFT_ASSETS_LOCATION assets ABSOLUTE)
configure_file(src/fixtures_location.h.in fixtures_location.h @ONLY)
target_include_directories(sft_unittests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

gtest_discover_tests(sft_unittests)
