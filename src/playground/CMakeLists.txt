sft_library(playground_lib
  ../../third_party/imgui/backends/imgui_impl_sdl.cpp
  ../../third_party/imgui/backends/imgui_impl_sdl.h
  ../../third_party/imgui/imgui.cpp
  ../../third_party/imgui/imgui.h
  ../../third_party/imgui/imgui_demo.cpp
  ../../third_party/imgui/imgui_draw.cpp
  ../../third_party/imgui/imgui_tables.cpp
  ../../third_party/imgui/imgui_widgets.cpp
  imgui_impl_sft.cc
  imgui_impl_sft.h
  imgui_shader.cc
  imgui_shader.h
  playground.cc
  playground.h
  sdl_utils.cc
  sdl_utils.h
)

target_link_libraries(playground_lib
  PUBLIC
    SDL2-static
    canvas
    model
)

target_include_directories(playground_lib
  PUBLIC
    ../../third_party/imgui
)

sft_executable(playground
  unittests.cc
  playground_test.cc
  playground_test.h
)

target_link_libraries(playground
  PRIVATE
    playground_lib
    gtest
    gtest_main
)

sft_executable(playground_benchmark
  benchmarks.cc
)

target_link_libraries(playground_benchmark
  PRIVATE
    benchmark_main
    rasterizer
)

get_filename_component(SFT_ASSETS_LOCATION ../../fixtures ABSOLUTE)
configure_file(fixtures_location.h.in fixtures_location.h @ONLY)
target_include_directories(playground PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(playground_benchmark PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

gtest_discover_tests(playground)
