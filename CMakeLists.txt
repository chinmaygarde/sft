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
  geom.h
  geom.cc
  image.h
  image.cc
  model.h
  model.cc
  renderer.h
  renderer.cc
)

target_link_libraries(sft_lib SDL2 tinyobjloader glm)

add_executable(sft main.cc)

target_link_libraries(sft sft_lib)

add_executable(sft_benchmark benchmark.cc)

target_link_libraries(sft_benchmark sft_lib benchmark::benchmark)
