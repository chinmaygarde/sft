sft_executable(playground
  ../third_party/imgui/backends/imgui_impl_sdl.cpp
  ../third_party/imgui/backends/imgui_impl_sdl.h
  ../third_party/imgui/imgui.cpp
  ../third_party/imgui/imgui.h
  ../third_party/imgui/imgui_demo.cpp
  ../third_party/imgui/imgui_draw.cpp
  ../third_party/imgui/imgui_tables.cpp
  ../third_party/imgui/imgui_widgets.cpp
  imgui_impl_sft.cc
  imgui_impl_sft.h
  imgui_shader.cc
  imgui_shader.h
  playground.cc
  playground.h
  playground_test.cc
  playground_test.h
  sdl_utils.cc
  sdl_utils.h
  unittests.cc
)

target_link_libraries(playground
  PUBLIC
    SDL2
    gtest
    gtest_main
    core
    geometry
    marl
    rasterizer
    canvas
    model
)

target_include_directories(playground
  PUBLIC
    ../third_party/imgui
)

get_filename_component(SFT_ASSETS_LOCATION ../fixtures ABSOLUTE)
configure_file(fixtures_location.h.in fixtures_location.h @ONLY)
target_include_directories(playground PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
